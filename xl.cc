
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

#define REQ_OBJ_ARG(I, VAR) \
if (args.Length() <= (I) || !args[I]->IsObject())                     \
    return ThrowException(Exception::TypeError(                         \
                  String::New("Argument " #I " must be an object")));   \
  Local<Object> VAR = Local<Object>::Cast(args[I]);                   

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

#define OPT_BOOL_ARG(I, VAR, DEFAULT)\
  bool VAR;\
  if (args.Length() <= (I)) \
    VAR = (DEFAULT); \
  else if (args[I]->IsBoolean()) \
  	VAR = args[I]->BooleanValue(); \
  else \
      return ThrowException(Exception::TypeError( \
                  String::New("Argument " #I " must be a boolean")));  


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

//xc_hvm_build_target_mem -lxenguest
//

unsigned long libxl_get_required_shadow_memory(unsigned long maxmem_kb, unsigned int smp_cpus)
{
    /* 256 pages (1MB) per vcpu,
       plus 1 page per MiB of RAM for the P2M map,
       plus 1 page per MiB of RAM to shadow the resident processes.
       This is higher than the minimum that Xen would allocate if no value
       were given (but the Xen minimum is for safety, not performance).
     */
    return 4 * (256 * smp_cpus + 2 * (maxmem_kb / 1024));
}

char *libxl_cpupoolid_to_name(libxl_ctx *ctx, uint32_t poolid)
{
    unsigned int len;
    char path[strlen("/local/pool") + 12];
    char *s;

    snprintf(path, sizeof(path), "/local/pool/%d/name", poolid);
    s = (char*)xs_read(ctx->xsh, XBT_NULL, path, &len);
    if (!s && (poolid == 0))
        return strdup("Pool-0");
    return s;
}

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

	static Handle<Object> domainInfoToObject(const libxl_dominfo& info) {
		Handle<Object> object = Object::New();
		object->Set(String::New("domId"), Integer::New(info.domid));
		object->Set(String::New("running"), Boolean::New(info.running));
		object->Set(String::New("blocked"), Boolean::New(info.blocked));
		object->Set(String::New("paused"), Boolean::New(info.paused));
		object->Set(String::New("shutdown"), Boolean::New(info.shutdown));
		object->Set(String::New("dying"), Boolean::New(info.dying));
		object->Set(String::New("shutdownReason"), Integer::New(info.shutdown_reason));
		object->Set(String::New("currentMemory"), Number::New(info.current_memkb));
		object->Set(String::New("maximumMemory"), Number::New(info.max_memkb));
		object->Set(String::New("cpuTime"), Number::New(info.cpu_time));
		object->Set(String::New("vcpuMaxId"), Integer::New(info.vcpu_max_id));
		object->Set(String::New("vcpuOnline"), Integer::New(info.vcpu_online));
		return object;
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
		OPT_BOOL_ARG(1, force, false);
		return Integer::New(libxl_domain_destroy(&context, id, force));
	};

	static Handle<Value> domainPause(const Arguments &args) {
		REQ_INT_ARG(0, id);
		return Integer::New(libxl_domain_pause(&context, id));
	};

	static Handle<Value> domainInfo(const Arguments &args) {
		REQ_INT_ARG(0, id);
		libxl_dominfo info = { 0 };
		if (0 == libxl_domain_info(&context, &info, id)) {
			return domainInfoToObject(info);
		}
		else {
			return Null();
		}
		return Integer::New(libxl_domain_pause(&context, id));
	};

	static Handle<Value> domainUnpause(const Arguments &args) {
		REQ_INT_ARG(0, id);
		return Integer::New(libxl_domain_unpause(&context, id));
	}

	static int consoleReady(libxl_ctx *ctx, uint32_t domid, void *priv) {
		return 0;
	}

	static void uuidToString(const libxl_uuid& uuid, char* buffer) {
		const unsigned char *d = (const unsigned char *)&uuid;
		sprintf(buffer, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
			d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]
		);
	}

	static Handle<Value> domainCreate(const Arguments& args) {
		uint32_t id = 0;
		libxl_domain_config config = { 0 };
		char* name;
		char uuidBuffer[36];
		REQ_OBJ_ARG(0, obj);

		Local<String> NAME = String::New("name"), HVM = String::New("hvm");

		libxl_init_create_info(&config.c_info);
		libxl_uuid_generate(&config.c_info.uuid);

		
		if (!obj->Has(NAME)) {
			uuidToString(config.c_info.uuid, uuidBuffer);
			name = uuidBuffer; //FiXME: Make this into a UUID
		}
		else {
			name = *String::Utf8Value(obj->Get(NAME)->ToString());
		}


		config.c_info.name = strdup(name);
		config.c_info.hvm = obj->Has(HVM) ? obj->Get(HVM)->BooleanValue() : 1;

		
		config.c_info.poolname = libxl_cpupoolid_to_name(&context, config.c_info.poolid);


		libxl_init_build_info(&config.b_info, &config.c_info);
		config.b_info.shadow_memkb = libxl_get_required_shadow_memory(config.b_info.max_memkb, config.b_info.max_vcpus);

		if (config.c_info.hvm == 1) {
			libxl_init_dm_info(&config.dm_info, &config.c_info, &config.b_info);
			config.dm_info.type = config.c_info.hvm ? XENFV : XENPV;
		}

		if (0 == libxl_domain_create_new(&context, &config, consoleReady, NULL, &id)) {
			return Integer::New(id);
		}
		else {
			return Null();
		}
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
		for (int i = 0; i < count; ++i) 
			out->Set(i, domainInfoToObject(domains[i]));
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

		NODE_SET_METHOD(target, "domainInfo", LibXL::domainInfo);
		NODE_SET_METHOD(target, "domainCreate", LibXL::domainCreate);
		NODE_SET_METHOD(target, "domainDestroy", LibXL::domainDestroy);
	
		LibXL::Init(target);
	}

	NODE_MODULE(xl, init)
}