This program implements a simple chat application (s-talk) through terminals. It allows for two users to communicate with each other over a network via text.

In order to use the application between computers, you'll probably need to have firewalls disabled.

You'll also need to know each other's machine name and the port numbers you will be communicating from. Any port number above about 2000 should be free to use, although to be 100% sure you would have to check what port numbers are reserved on your machine. Since port number must be specified, it also possible to communicate between terminals on the same machine--just pick two different port numbers.

After making the executable, here is the format to follow in the terminal:
./s-talk [your port number] [remote machine name] [remote port number]

Once both users have executed the above command appropriately, they should be connected and be able to chat with each other. Simply type your message and press enter and it will be sent immediately.

This application has only been tested on Linux. It may work on other POSIX based systems, but there are no guarantees.