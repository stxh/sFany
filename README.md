sFany
=====

Recently I want to start nginx server as a windows service. But it is not windows service itself.
Some solutions use srvany.exe from Microsoft. Yes it works. But if you want start PHP as server as
same time, srvany can't do it.

So I write my srvany, it is sFany here.

Usage
=====
	sFany [option]

	Options:
        -v      View services list
        -i      Install service
        -u      Uninstall service
        -k      Kill/stop service
        -s      Start service

Setting
=======
  sFany use ini configuaration file like this:

	[Service]
	Name=sFany
	server1 = path_to\server1.exe -c command line
	server2 = path_to\server2.exe -c command line
	......

Contact
=======
Write an email to stxh007@gmail.com

License
=======
(The MIT License)

Copyright (c) 2012 stxh007@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
