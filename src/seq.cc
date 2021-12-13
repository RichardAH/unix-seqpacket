
#if defined(_WIN32) || defined(_MSC_VER)
#define WIN
#endif


#include <node.h>
#include <v8.h>
#include <nan.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace v8;

#define THROW(x)\
{\
    isolate->ThrowException(Exception::TypeError(\
        String::NewFromUtf8(isolate, x)));\
    return;\
}

namespace unix_seqpacket
{
    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Context;
    using v8::Function;
    using v8::FunctionTemplate;
    using v8::Integer;
    using v8::Null;
    using v8::Object;
    using v8::Persistent;

    void Open(const FunctionCallbackInfo<Value>& args)
    {
        Isolate* isolate = args.GetIsolate();
#ifdef WIN
        args.GetReturnValue().Set(Undefined(isolate));
#else
        if (args.Length() != 1 || !args[0]->IsString())
            THROW("wrong number of arguments, expecting socket path string only");

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, *Nan::Utf8String(args[0]), sizeof(addr.sun_path) - 1);

        // create socket
        int sub_fd = -1;
        if ((sub_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) < 0)
            THROW("OS refused to create socket of form sock_seqpacket");

        // connect
        if (connect(sub_fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
            THROW("could not connect to specified socket path (does it exist? do you have permissions?)");


        if (sub_fd < 0)
            THROW("unknown error, could not create or connect to sock_seqpacket socket");

        args.GetReturnValue().Set(Integer::New(isolate, sub_fd));
#endif        
    }

    void Close(const FunctionCallbackInfo<Value>& args)
    {
        Isolate* isolate = args.GetIsolate();
#ifdef WIN
        args.GetReturnValue().Set(Undefined(isolate));
#else
        if (args.Length() != 1 || !args[0]->IsNumber())
            THROW("wrong number of arguments, expecting open socket fd only");

        int fd = (int)(args[0].As<Number>()->Value());
        close(fd);

        args.GetReturnValue().Set(Boolean::New(isolate, true));
#endif
    }

    void _GetFdBytes(int fd, const FunctionCallbackInfo<Value>& args) 
    {
        Isolate* isolate = args.GetIsolate();
#ifdef WIN
        args.GetReturnValue().Set(Undefined(isolate));
#else
     
        int bytes_available = 0;
        ioctl(fd, FIONREAD, &bytes_available);

        if (bytes_available > 0) {
            Local<ArrayBuffer> abuf = ArrayBuffer::New(isolate, bytes_available);
            char* data = (char*)(abuf->GetContents().Data());
            read(fd, data, bytes_available);
            args.GetReturnValue().Set(abuf);
            return; 
        } 

        Local<ArrayBuffer> abuf = ArrayBuffer::New(isolate, 0);
        args.GetReturnValue().Set(abuf);
        return;
#endif
    }


    void GetFdBytes(const FunctionCallbackInfo<Value>& args)
    {
#ifdef WIN
        args.GetReturnValue().Set(Undefined(isolate));
#else
        if (args.Length() != 1) {
            args.GetReturnValue().Set(Undefined(args.GetIsolate()));
            return;
        }     
        int fd = (int)(args[0].As<Number>()->Value());

        _GetFdBytes(fd, args);
        return;
#endif        
    }    

    void Initialize(Local<Object> exports)
    {
        NODE_SET_METHOD(exports, "open", Open);
        NODE_SET_METHOD(exports, "close", Close);
        NODE_SET_METHOD(exports, "available", GetFdBytes);
    }

}

NODE_MODULE(unix_seqpacket, unix_seqpacket::Initialize)
