#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://en.wikipedia.org/wiki/ANSI_escape_code

from sys import stdout

_stream = stdout

def set_stream(stream):
    global _stream
    _stream = stream

def move_cursor_up(n=1):
    _stream.write('\x1b[%dA' % n)

def move_cursor_down(n=1):
    _stream.write('\x1b[%dB' % n)

def move_cursor_forward(n=1):
    _stream.write('\x1b[%dC' % n)

def move_cursor_back(n=1):
    _stream.write('\x1b[%dD' % n)

def earse_display(i=0):
    '''
    The value of i:

    * 0 - clear from cursor to end of screen
    * 1 - clear from cursor to beginning of screen
    * 2 - clear entire screen
    '''
    _stream.write('\x1b[%dJ' % i)

def earse_entire_display():
    earse_display(2)

def earse_in_line(i=0):
    '''
    The value of i:

    * 0 - clear from cursor to end of line
    * 1 - clear from cursor to beginning of line
    * 2 - clear entire line
    '''
    _stream.write('\x1b[%dK' % i)

def earse_entire_line():
    earse_in_line(2)

def set_cursor_position(n=1, m=1):
    _stream.write('\x1b[%d;%dH' % (n, m))

def select_graphic_rendition(*args):

    # http://en.wikipedia.org/wiki/ANSI_escape_code#graphics

    if not args:
        args = (0, )

    _stream.write('\x1b[')
    _stream.write(';'.join(str(a) for a in args if a is not None))
    _stream.write('m')

def set_attribute(attr=0):
    select_graphic_rendition(attr)

def set_color(color=9, attr=None):
    select_graphic_rendition(color+30, attr)

def set_background_color(color=9, attr=None):
    select_graphic_rendition(color+40, attr)

def set_xterm256_color(color=None, attr=None):
    # The Color Chart: http://www.calmar.ws/vim/256-xterm-24bit-rgb-color-chart.html
    select_graphic_rendition(38, 5, color)

def set_xterm256_background_color(color=None, attr=None):
    select_graphic_rendition(48, 5, color)

def save_cursor_position():
    _stream.write('\x1b[s')

def restore_cursor_position():
    _stream.write('\x1b[u')

def hide_cursor():
    _stream.write('\x1b[?25l')

def show_cursor():
    _stream.write('\x1b[?25h')

def print_pyramid(n=10):
    for i in range(n):
        _stream.write(str.center('*'*(i*2+1), n*2))
        _stream.write('\n')

if __name__ == '__main__':

    earse_entire_display()
    set_cursor_position()
    print_pyramid()
    stdout.write('\n')

    stdout.write('Hello, World!')
    move_cursor_back(6)
    set_attribute(4)
    stdout.write('Mosky')
    set_attribute()
    stdout.write('\n\n')
