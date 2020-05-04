# Sdebug

Sdebug (Swoole debug) is a fork of xdebug in order for xdebug to support Swoole.

## Installation

```bash
phpize \
&& ./configure --enable-swoole \
&& make clean \
&& make all \
&& make install
```

> Do not use rebuild.sh to compile extensions, as compilation can become more restrictive and may cause compilation failure

Then you need to add the following line to your `php.ini` file, or create a new Sdebug specific ini file `sdebug.ini` in the `conf.d` directory. In either case, it
needs the following line added:

```ini
zend_extension=xdebug.so
```
