# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Spawns esp-emu when tests run with `--embedded-services idf,serial`.

The dut then talks to the emulator's UART over `--port socket://127.0.0.1:5555`.
QEMU runs use the pytest-embedded qemu service and skip this fixture entirely.
"""

import select
import socket
import subprocess
import time
from pathlib import Path

import pexpect
import pytest

# ponytail: fixed port, parametrize if two emu jobs ever share a runner
EMU_UART_HOST = '127.0.0.1'
EMU_UART_PORT = 5555

_MENU_PATTERN = "Here's the test menu, pick your combo:(.+)Enter test for running."


def _emu_parse_test_menu(self, ready_line='', pattern=_MENU_PATTERN, trigger=''):
    """esp-emu's --uart-tcp discards UART output printed before the client
    attaches, so the one-shot 'Press ENTER' boot banner may never arrive
    (unlike QEMU, which pytest-embedded launches and attaches to before
    boot). Wait briefly for the banner, then poke Unity with a newline to
    print the menu regardless."""
    try:
        self.expect_exact('Press ENTER to see the list of tests', timeout=10)
    except pexpect.TIMEOUT:
        pass  # banner lost before the client attached
    res = self.confirm_write('', expect_pattern=_MENU_PATTERN, timeout=5, retry_times=6)
    # the menu print ends with the ready prompt and was just consumed;
    # SerialDut has no hard_reset to produce a fresh one, so skip it once
    self._ignore_first_ready_pattern = True
    return self._parse_unity_menu_from_str(res.group(1).decode('utf8'))


def _uart_tcp_listening(port: int) -> bool:
    """Check for a LISTEN socket without connecting — esp-emu's --uart-tcp
    bridge serves the connected client, so a probe connection would steal
    the slot from the dut's socket:// connection."""
    try:
        with open('/proc/net/tcp') as f:  # Linux (CI)
            return any(
                int(line.split()[1].split(':')[1], 16) == port and line.split()[3] == '0A'
                for line in f.readlines()[1:]
            )
    except FileNotFoundError:  # macOS
        return subprocess.run(['lsof', f'-iTCP:{port}', '-sTCP:LISTEN'], capture_output=True).returncode == 0


@pytest.fixture(autouse=True)
def esp_emu(request, monkeypatch):
    services = [s.strip() for s in (request.config.getoption('embedded_services') or '').split(',')]
    if 'serial' not in services:
        yield None
        return

    from pytest_embedded_idf.unity_tester import IdfUnityDutMixin

    monkeypatch.setattr(IdfUnityDutMixin, '_parse_test_menu', _emu_parse_test_menu)

    # pyserial's socket:// handler reports in_waiting as 0 or 1 (a bare
    # select()), so pytest-embedded's read_all()-based reader drains one
    # byte per loop (~20 B/s). Report the real readable byte count.
    from serial.urlhandler.protocol_socket import Serial as _SocketSerial

    def _in_waiting(self):
        if not self.is_open:
            return 0
        readable, _, _ = select.select([self._socket], [], [], 0)
        if not readable:
            return 0
        return len(self._socket.recv(4096, socket.MSG_PEEK))

    monkeypatch.setattr(_SocketSerial, 'in_waiting', property(_in_waiting))

    target = request.config.getoption('target') or 'esp32c3'
    firmware = Path(__file__).parent / 'build' / 'merged-binary.bin'
    if not firmware.exists():
        pytest.fail(f'{firmware} missing — build with `idf.py build merge-bin`')

    # emulator's own stdout/stderr, next to the pytest-embedded logs (CI artifact)
    log_dir = Path('pytest_embedded_log') / 'esp-emu'
    log_dir.mkdir(parents=True, exist_ok=True)
    log_file = open(log_dir / f'{request.node.name}.txt', 'wb')

    proc = subprocess.Popen(
        [
            'esp-emu',
            '--chip', target,
            '--firmware', str(firmware),
            '--uart-tcp', f'{EMU_UART_HOST}:{EMU_UART_PORT}',
            # tests need no external connectivity; on noisy CI LANs the
            # relayed multicast floods the emulated NIC (RX-drop storms,
            # ~5x slower emulation, lwIP allocs tripping Unity leak checks)
            '--net', 'user,restrict=yes',
        ],
        stdout=log_file,
        stderr=subprocess.STDOUT,
    )

    deadline = time.monotonic() + 30
    while not _uart_tcp_listening(EMU_UART_PORT):
        if proc.poll() is not None or time.monotonic() > deadline:
            proc.kill()
            log_file.close()
            pytest.fail('esp-emu did not open its UART TCP server')
        time.sleep(0.2)

    yield proc

    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
    log_file.close()
