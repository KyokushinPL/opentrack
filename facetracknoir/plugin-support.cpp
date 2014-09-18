#include "plugin-support.h"
#include <QCoreApplication>

#if !(defined(_WIN32))
#   include <dlfcn.h>
#endif

Plugin::Plugin(const QString& filename)
{
#if defined(_WIN32)
    QString fullPath = QCoreApplication::applicationDirPath() + "/" + filename;
    handle = std::make_shared<QLibrary>(fullPath);
    qDebug() << handle->errorString();
    Dialog = (DIALOG_FUNPTR) handle->resolve(MAYBE_STDCALL_UNDERSCORE "GetDialog" CALLING_CONVENTION_SUFFIX_VOID_FUNCTION);
    qDebug() << handle->errorString();
    Constructor = (CTOR_FUNPTR) handle->resolve(MAYBE_STDCALL_UNDERSCORE "GetConstructor" CALLING_CONVENTION_SUFFIX_VOID_FUNCTION);
    qDebug() << handle->errorString();
    Metadata = (METADATA_FUNPTR) handle->resolve(MAYBE_STDCALL_UNDERSCORE "GetMetadata" CALLING_CONVENTION_SUFFIX_VOID_FUNCTION);
    qDebug() << handle->errorString();
#else
    QByteArray latin1 = QFile::encodeName(filename);
    handle = dlopen(latin1.constData(), RTLD_NOW |
#   ifdef __linux
                    RTLD_DEEPBIND
#   elif defined(__APPLE__)
                    RTLD_LOCAL|RTLD_FIRST|RTLD_NOW
#   else
                    0
#   endif
                    );
    if (handle)
    {
        fprintf(stderr, "Error, if any: %s\n", dlerror());
        fflush(stderr);
        Dialog = (DIALOG_FUNPTR) dlsym(handle, "GetDialog");
        fprintf(stderr, "Error, if any: %s\n", dlerror());
        fflush(stderr);
        Constructor = (CTOR_FUNPTR) dlsym(handle, "GetConstructor");
        fprintf(stderr, "Error, if any: %s\n", dlerror());
        fflush(stderr);
        Metadata = (METADATA_FUNPTR) dlsym(handle, "GetMetadata");
        fprintf(stderr, "Error, if any: %s\n", dlerror());
        fflush(stderr);
    } else {
        fprintf(stderr, "Error, if any: %s\n", dlerror());
        fflush(stderr);
    }
#endif
}

Plugin::~Plugin()
{
#if defined(_WIN32)
    handle->unload();
#else
    if (handle)
        (void) dlclose(handle);
#endif
}
