#pragma once

#if defined(_WIN32)
#   define CALLING_CONVENTION_SUFFIX_VOID_FUNCTION "@0"
#   ifdef _MSC_VER
#       error "No support for MSVC anymore"
#else
#       define MAYBE_STDCALL_UNDERSCORE ""
#   endif
#else
#   define CALLING_CONVENTION_SUFFIX_VOID_FUNCTION ""
#   define MAYBE_STDCALL_UNDERSCORE ""
#endif

#include <cstdio>
#include <memory>

#include <QWidget>
#include <QDebug>
#include <QString>
#include <QLibrary>
#include <QFrame>
#include "ftnoir_tracker_base/ftnoir_tracker_base.h"
#include "ftnoir_filter_base/ftnoir_filter_base.h"
#include "ftnoir_protocol_base/ftnoir_protocol_base.h"

#if defined(_WIN32)
#   define CALLING_CONVENTION __stdcall
#else
#   define CALLING_CONVENTION
#endif

struct Metadata;

extern "C" typedef void* (CALLING_CONVENTION * CTOR_FUNPTR)(void);
extern "C" typedef Metadata* (CALLING_CONVENTION* METADATA_FUNPTR)(void);
extern "C" typedef void* (CALLING_CONVENTION* DIALOG_FUNPTR)(void);

template<typename t> using ptr = typename std::shared_ptr<t>;

class Plugin {
public:
    Plugin(const QString& filename);
    explicit Plugin() : Dialog(nullptr), Constructor(nullptr), Metadata(nullptr), handle() {}
    virtual ~Plugin();
    
    DIALOG_FUNPTR Dialog;
    CTOR_FUNPTR Constructor;
    METADATA_FUNPTR Metadata;
private:
#if defined(_WIN32)
    ptr<QLibrary> handle;
#else
    void* handle;
#endif
};

struct Metadata
{
    Metadata() {}
    virtual ~Metadata() {}

    virtual void getFullName(QString *strToBeFilled) = 0;
    virtual void getShortName(QString *strToBeFilled) = 0;
    virtual void getDescription(QString *strToBeFilled) = 0;
    virtual void getIcon(QIcon *icon) = 0;
};

// merely to break a circular header dependency -sh
class IDynamicLibraryProvider {
public:
    virtual Plugin current_tracker1() = 0;
    virtual Plugin current_tracker2() = 0;
    virtual Plugin current_protocol() = 0;
    virtual Plugin current_filter() = 0;
    virtual QFrame* get_video_widget() = 0;
};
