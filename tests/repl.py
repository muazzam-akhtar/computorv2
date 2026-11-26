#!/usr/bin/env python3
"""Drives ./computorv2 in a real pseudo-terminal: Ctrl-C, Ctrl-D, exit keywords, recovery after errors.
Usage:  python3 tests/repl.py        (after `make`)"""
import os, pty, select, signal, sys, time

def session(keys):
    pid, fd = pty.fork()
    if pid == 0:
        os.execv('./computorv2', ['./computorv2'])
    out = b''
    def drain(t):
        nonlocal out
        end = time.time() + t
        while time.time() < end:
            r, _, _ = select.select([fd], [], [], 0.05)
            if r:
                try: out += os.read(fd, 4096)
                except OSError: return
    drain(0.4)
    for k in keys:
        os.write(fd, k)
        drain(0.25)
    drain(0.3)
    time.sleep(0.4)                       # let the child finish exiting before we look
    done, status = os.waitpid(pid, os.WNOHANG)
    alive = (done == 0)
    if alive:
        os.kill(pid, signal.SIGKILL); os.waitpid(pid, 0)
    return alive, out.decode(errors='replace').replace('\r', '')

CASES = [
    ('Ctrl-C at the prompt exits',            [b'1 + 1\n', b'\x03'],             False, '2'),
    ('Ctrl-C in the middle of a line exits',  [b'2 +', b'\x03'],                 False, ''),
    ('Ctrl-D exits',                          [b'x = 2\n', b'\x04'],             False, ''),
    ('"exit" exits',                          [b'x = 2\n', b'exit\n'],           False, ''),
    ('"QUIT" exits (case-insensitive)',       [b'QUIT\n'],                       False, ''),
    ('blank lines are ignored',               [b'\n', b'   \n', b'3 + 4 = ?\n', b'exit\n'], False, '7'),
    ('errors do not stop the REPL',           [b'1 +\n', b'oops(\n', b'2 * 21 = ?\n', b'exit\n'], False, '42'),
    ('nothing runs after exit',               [b'exit\n', b'9 + 9 = ?\n'],       False, None),
    ('solve works interactively',             [b'x^2 - 4 = 0 ?\n', b'exit\n'],   False, 'Two solutions: R'),
]

bad = 0
for label, keys, want_alive, want_text in CASES:
    alive, out = session(keys)
    ok = (alive == want_alive)
    if want_text is not None and want_text not in out:
        ok = False
    if want_text is None and '18' in out:
        ok = False
    print('%s  %s' % ('ok  ' if ok else 'FAIL', label))
    if not ok:
        bad += 1
        print('      alive=%s output=%r' % (alive, out))
print('-- %d passed, %d failed' % (len(CASES) - bad, bad))
sys.exit(1 if bad else 0)
