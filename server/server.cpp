#include "stdafx.h"
#include "server.h"

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#include <algorithm>

#define BUF_SIZE 1024

SetupServer::SetupServer(void)
{
	pipe_handle = CreateNamedPipe(TEXT("\\\\.\\pipe\\brindal_Pipe"),
		PIPE_ACCESS_DUPLEX, // two way
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1, // one instance of the pipe
		1024 * 16, // outbound buf
		1024 * 16, //inbound buf
		NMPWAIT_USE_DEFAULT_WAIT, //wait time
		NULL); // default securities attributes

	memset(buf, 0, sizeof(buf));
	read_dw = 0;
	write_dw = 0;
}


int SetupServer::start_server()
{
    while (pipe_handle != INVALID_HANDLE_VALUE)
    {
        if (ConnectNamedPipe(pipe_handle, NULL) != FALSE)   // wait for someone to connect to the pipe
        {
            std::string recv_buf; // received buffer
            std::string res_str; //response sting

            recv_buf.clear();
            res_str.clear();

            while (ReadFile(pipe_handle, buf, sizeof(buf) - 1, &read_dw, NULL) != FALSE)
            {
                /* add terminating zero */
                buf[read_dw] = '\0';

                /* do something with data in buf */
                printf("%s", buf);
                recv_buf  =   b u f ; 
                recv_buf. e r a s e ( s t d : : r e m o v e (recv_buf. b e g i n ( ) ,  recv_buf. e n d ( ) ,   ' \ n ' ) ,  recv_buf. e n d ( ) ) ; 
                res_str = response_to_client(recv_buf);
                write_to_client(res_str);
            }
        }

        DisconnectNamedPipe(pipe_handle);
    }

    return 0;
}


std::string SetupServer::response_to_client(std::string recv_msg)
{
    std::string res;
    res.clear();
    if (recv_msg.compare("--help") == 0)
    {
		res = "****help****\n\thello: to say hi\n\twrite:to write\n\texit:to disconnect;\n\n";
    }
    else if (recv_msg.compare("hello") == 0)
    {
        res = "****hello****\n\tHi, How are you?\n\n";
    }
    else if (recv_msg.compare("exit") == 0)
    {
        res = "disconnect\n\n";
    }
    else
    {
        res = "**unknown message...Please check the help by sending --help!!";
    }

    return res;
}


void SetupServer::write_to_client(std::string msg)
{
    WriteFile(pipe_handle,
              msg.c_str(),
              100,   // allowing only 100 char max in single message
              &write_dw,
              NULL);
    return;
}

SetupServer::~SetupServer(void)
{

}
