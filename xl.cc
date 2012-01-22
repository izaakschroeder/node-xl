
#include <cstring>

#include <v8.h>

#include <node.h>
#include <node_buffer.h>

extern "C" {
#include <libxl.h>
};

#define THROW_ERROR(TYPE, STR)                                          \
	return ThrowException(Exception::TYPE(String::New(STR)));

#define REQ_ARGS(N)                                                     \
  if (args.Length() < (N))                                              \
    return ThrowException(Exception::TypeError(                         \
                             String::New("Expected " #N "arguments"))); 

#define OPT_STR_ARG(I, VAR, DEFAULT)                                    \
  const char* VAR;                                                              \
  if (args.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (args[I]->IsString()) {                                      \
    VAR = *String::Utf8Value(args[I]->ToString());                                        \
  } else {                                                              \
    return ThrowException(Exception::TypeError(                         \
              String::New("Argument " #I " must be a string"))); \
  }

#define REQ_STR_ARG(I, VAR)                                             \
  const char* VAR;														\
  if (args.Length() <= (I) || !args[I]->IsString())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a string")));    \
  String::Utf8Value _VAR(args[I]->ToString());		\
  VAR = *_VAR;

#define REQ_INT_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (args.Length() <= (I) || !args[I]->IsInt32())                      \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be an integer")));  \
  VAR = args[I]->Int32Value();

#define REQ_BOOL_ARG(I, VAR)                                             \
  int VAR;                                                              \
  if (args.Length() <= (I) || !args[I]->IsBoolean())                      \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a boolean")));  \
  VAR = args[I]->BooleanValue();


#define REQ_FN_ARG(I, VAR)                                              \
  v8::Local<v8::Function> VAR;                                                           \
  if (args.Length() <= (I) || !args[I]->IsFunction())                   \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be a function")));  \
  VAR = v8::Local<v8::Function>::Cast(args[I]);

#define REQ_BUF_ARG(I, VARBLOB, VARLEN)                                             \
  const char* VARBLOB;													\
  size_t VARLEN;	                                                    \
  if (args.Length() <= (I) || (!args[I]->IsString() && !Buffer::HasInstance(args[I])))                      \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be an buffer")));  \
 if (args[I]->IsString()) { \
	String::AsciiValue string(args[I]->ToString()); \
	length = string.length(); \
	blob = *string; \
} else if (Buffer::HasInstance(args[I])) { \
	Local<Object> bufferIn=args[I]->ToObject(); \
	length = Buffer::Length(bufferIn); \
	blob = Buffer::Data(bufferIn); \
}


                  
#define OPT_INT_ARG(I, VAR, DEFAULT)                                    \
  int VAR;                                                              \
  if (args.Length() <= (I)) {                                           \
    VAR = (DEFAULT);                                                    \
  } else if (args[I]->IsInt32()) {                                      \
    VAR = args[I]->Int32Value();                                        \
  } else {                                                              \
    return ThrowException(Exception::TypeError(                         \
              String::New("Argument " #I " must be an integer"))); \
  }

                  
using namespace node;
using namespace v8;

class LibXL : ObjectWrap {
public:
	
	
	static libxl_ctx context;
	static xentoollog_logger logger; 
	
	
	LibXL() : ObjectWrap() {
		
	}
	
	~LibXL() {
		
	}
	
	operator libxl_ctx* () const {
		return &context;
	}
	
	
	
	static void logMessage(struct xentoollog_logger *logger, xentoollog_level level, int errnoval, const char *context, const char *format, va_list al) {
		
	};

	static void logProgress(struct xentoollog_logger *logger, const char *context, const char *doing_what, int percent, unsigned long done, unsigned long total) {
		
	};

	static void logDestroy(struct xentoollog_logger *logger) {
		
	};

	static void Init(Handle<Object> target) {
		
		
		if (libxl_ctx_init(&context, LIBXL_VERSION, &logger)) {
			printf("FUUU\n");
			return;
		}


	};

	
	static Handle<Value> domainResume(const Arguments &args) {
		REQ_INT_ARG(0, id);
		return Integer::New(libxl_domain_resume(&context, id));
	};

	static Handle<Value> domainShutdown(const Arguments &args) {
		REQ_INT_ARG(0, id);
		REQ_INT_ARG(1, req);
		return Integer::New(libxl_domain_shutdown(&context, id, req));
	};

	static Handle<Value> domainDestroy(const Arguments &args) {
		REQ_INT_ARG(0, id);
		REQ_BOOL_ARG(1, force);
		return Integer::New(libxl_domain_destroy(&context, id, force));
	};

	static Handle<Value> domainPause(const Arguments &args) {
		REQ_INT_ARG(0, id);
		return Integer::New(libxl_domain_pause(&context, id));
	};

	static Handle<Value> domainUnpause(const Arguments &args) {
		REQ_INT_ARG(0, id);
		return Integer::New(libxl_domain_unpause(&context, id));
	}

	/*
	static Handle<Value> domainSuspend(const Arguments &args) {
		libxl_domain_suspend_info info = { 0, callback };
		libxl_domain_suspend(&context, &info, id, fd);
	};
	*/

	
	/*
	

	static Handle<Value> domainPreserve(const Arguments &args) {
		REQ_INT_ARG(0, id);
		libxl_domain_preserve(&context, id, &info, nameSuffix, newUUID);
	}

	static Handle<Value> domainRename(const Arguments &args) {
		libxl_domain_rename(&context, id, oldName, newName, transaction);
	}

	

	static Handle<Value> domainCoreDump(const Arguments &args) {
		libxl_domain_core_dump
	}

	static Handle<Value> domainSetMaximumMemory(const Arguments &args) {
		libxl_domain_setmaxmem
	}

	static Handle<Value> domainSetMemoryTarget(const Arguments &args) {
		libxl_set_memory_target
	}

	static Handle<Value> domainGetMemoryTarget(const Arguments &args) {
		libxl_get_memory_target
	}

	static Handle<Value> runBootloader(const Arguments &args) {
		libxl_run_bootloader
	}

	*/
	
	static Handle<Value> maxCPUs(Local<String> property, const AccessorInfo& info) {
		return Integer::New(libxl_get_max_cpus(&context));
	};
	
	static Handle<Value> domains(Local<String> property, const AccessorInfo& info) {
		int count = 0;
		libxl_dominfo* domains;
		domains = libxl_list_domain(&context, &count);
		Local<Array> out = Array::New(count);
		for (int i = 0; i < count; ++i) {
			Handle<Object> object = Object::New();
			object->Set(String::New("domId"), Integer::New(domains[i].domid));
			object->Set(String::New("running"), Boolean::New(domains[i].running));
			object->Set(String::New("blocked"), Boolean::New(domains[i].blocked));
			object->Set(String::New("paused"), Boolean::New(domains[i].paused));
			object->Set(String::New("shutdown"), Boolean::New(domains[i].shutdown));
			object->Set(String::New("dying"), Boolean::New(domains[i].dying));
			object->Set(String::New("shutdownReason"), Integer::New(domains[i].shutdown_reason));
			object->Set(String::New("currentMemory"), Number::New(domains[i].current_memkb));
			object->Set(String::New("maximumMemory"), Number::New(domains[i].max_memkb));
			object->Set(String::New("cpuTime"), Number::New(domains[i].cpu_time));
			object->Set(String::New("vcpuMaxId"), Integer::New(domains[i].vcpu_max_id));
			object->Set(String::New("vcpuOnline"), Integer::New(domains[i].vcpu_online));
			out->Set(i, object);
		}
		return out;
	}

	static Handle<Value> cpuPools(Local<String> property, const AccessorInfo& info) {
		int count = 0;
		libxl_cpupoolinfo* cpuPools;
		cpuPools = libxl_list_cpupool(&context, &count);
		Local<Array> out = Array::New(count);
		for (int i = 0; i < count; ++i) {
			Handle<Object> object = Object::New();
			object->Set(String::New("poolId"), Integer::New(cpuPools[i].poolid));
			object->Set(String::New("schedulerId"), Integer::New(cpuPools[i].sched_id));
			object->Set(String::New("domainCount"), Integer::New(cpuPools[i].n_dom));
			object->Set(String::New("cpuMap"), Null()); //FIXME: This needs implementing
			out->Set(i, object);
		}
		return out;
	}

	static Handle<Value> version(Local<String> property, const AccessorInfo& accessorInfo) {
		Handle<Object> object = Object::New();
		const libxl_version_info info = *libxl_get_version_info(&context);
		object->Set(String::New("xenVersionMajor"), Integer::New(info.xen_version_major));
		object->Set(String::New("xenVersionMinor"), Integer::New(info.xen_version_minor));
		object->Set(String::New("xenVersionExtra"), String::New(info.xen_version_extra));
		object->Set(String::New("compiler"), String::New(info.compiler));
		object->Set(String::New("compileBy"), String::New(info.compile_by));
		object->Set(String::New("compileDomain"), String::New(info.compile_domain));
		object->Set(String::New("compileDate"), String::New(info.compile_date));
		object->Set(String::New("capabilities"), String::New(info.capabilities));
		object->Set(String::New("virtStart"), Number::New(info.virt_start));
		object->Set(String::New("pageSize"), Number::New(info.pagesize));
		object->Set(String::New("commandline"), String::New(info.commandline));
		return object;
	}
	
};

libxl_ctx LibXL::context = { 0 };
xentoollog_logger LibXL::logger = { LibXL::logMessage, LibXL::logProgress, LibXL::logDestroy };

extern "C" {
	
	static void init (Handle<Object> target)
	{
				
		//http://linux.die.net/man/3/libmagic
		NODE_DEFINE_CONSTANT(target, XENFV);
		NODE_DEFINE_CONSTANT(target, XENPV);

		NODE_DEFINE_CONSTANT(target, LIBXL_CONSTYPE_SERIAL);
		NODE_DEFINE_CONSTANT(target, LIBXL_CONSTYPE_PV);
		
		NODE_DEFINE_CONSTANT(target, LIBXL_CONSBACK_XENCONSOLED);
		NODE_DEFINE_CONSTANT(target, LIBXL_CONSBACK_IOEMU);
		
		NODE_DEFINE_CONSTANT(target, DISK_FORMAT_UNKNOWN);
		NODE_DEFINE_CONSTANT(target, DISK_FORMAT_QCOW);
		NODE_DEFINE_CONSTANT(target, DISK_FORMAT_QCOW2);
		NODE_DEFINE_CONSTANT(target, DISK_FORMAT_VHD);
		NODE_DEFINE_CONSTANT(target, DISK_FORMAT_RAW);
		NODE_DEFINE_CONSTANT(target, DISK_FORMAT_EMPTY);

		NODE_DEFINE_CONSTANT(target, DISK_BACKEND_UNKNOWN);
		NODE_DEFINE_CONSTANT(target, DISK_BACKEND_PHY);
		NODE_DEFINE_CONSTANT(target, DISK_BACKEND_TAP);
		NODE_DEFINE_CONSTANT(target, DISK_BACKEND_QDISK);

		NODE_DEFINE_CONSTANT(target, NICTYPE_IOEMU);
		NODE_DEFINE_CONSTANT(target, NICTYPE_VIF);

		NODE_DEFINE_CONSTANT(target, ERROR_NONSPECIFIC);
		NODE_DEFINE_CONSTANT(target, ERROR_VERSION);
		NODE_DEFINE_CONSTANT(target, ERROR_FAIL);
		NODE_DEFINE_CONSTANT(target, ERROR_NI);
		NODE_DEFINE_CONSTANT(target, ERROR_NOMEM);
		NODE_DEFINE_CONSTANT(target, ERROR_INVAL);
		NODE_DEFINE_CONSTANT(target, ERROR_BADFAIL);
		NODE_DEFINE_CONSTANT(target, ERROR_GUEST_TIMEDOUT);
		

		NODE_DEFINE_CONSTANT(target, LIBXL_ACTION_DESTROY);
		NODE_DEFINE_CONSTANT(target, LIBXL_ACTION_RESTART);
		NODE_DEFINE_CONSTANT(target, LIBXL_ACTION_RESTART_RENAME);
		NODE_DEFINE_CONSTANT(target, LIBXL_ACTION_PRESERVE);
		NODE_DEFINE_CONSTANT(target, LIBXL_ACTION_COREDUMP_DESTROY);
		NODE_DEFINE_CONSTANT(target, LIBXL_ACTION_COREDUMP_RESTART);

		NODE_DEFINE_CONSTANT(target, LIBXL_EVENT_DOMAIN_DEATH);
		NODE_DEFINE_CONSTANT(target, LIBXL_EVENT_DISK_EJECT);

		NODE_DEFINE_CONSTANT(target, POWER_BUTTON);
		NODE_DEFINE_CONSTANT(target, SLEEP_BUTTON);

		NODE_DEFINE_CONSTANT(target, LIBXL_VERSION);

		NODE_DEFINE_CONSTANT(target, XL_SUSPEND_DEBUG);
		NODE_DEFINE_CONSTANT(target, XL_SUSPEND_LIVE);

		NODE_DEFINE_CONSTANT(target, LIBXL_PCI_FUNC_ALL);

		NODE_DEFINE_CONSTANT(target, LIBXL_CPUARRAY_INVALID_ENTRY);
		

		target->SetAccessor(String::NewSymbol("maxCPUs"), LibXL::maxCPUs, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
		target->SetAccessor(String::NewSymbol("domains"), LibXL::domains, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);
		target->SetAccessor(String::NewSymbol("cpuPools"), LibXL::cpuPools, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);

		target->SetAccessor(String::NewSymbol("version"), LibXL::version, NULL, Handle<Value>(), PROHIBITS_OVERWRITING, ReadOnly);


		//NODE_SET_METHOD(target, "create", Magic::create);
		
		LibXL::Init(target);
	}

	NODE_MODULE(xl, init)
}