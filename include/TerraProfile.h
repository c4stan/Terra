#pragma once

// Terra
#include <Terra.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
typedef int64_t TerraClockTime;
#else
typedef clock_t TerraClockTime;
#endif

void                terra_clock_init();
TerraClockTime      terra_clock();
double              terra_clock_to_ms ( TerraClockTime delta_time );

#ifdef TERRA_PROFILE

// Dynamic resizing of sessions/targets is disabled.
// These numbers are usually (always?) knwon at compile time.
#define TERRA_PROFILE_SESSIONS             128
#define TERRA_PROFILE_TARGETS_PER_SESSION  128

// Threads that want to collect samples have to register before starting to collect.
// Collected data is grouped in sessions and targets.
// 99% of situations only require one session. A two sessions example might be something like profiling two different job systems at the same time.
// Each component for which data is to be collected shall be a target. For each target, a data collection buffer is created for each thread.


// Stats are always computed in double floating precision.
typedef struct {
    double avg;
    double var;
    double min;
    double max;
    double sum;
    double n;
} TerraProfileStats;

typedef __declspec ( align ( 64 ) ) struct {
    // Thread local buffer stats.
    TerraProfileStats stats;
    // Buffer size.
    size_t size;
    size_t cap;
    // Buffer data.
    TerraClockTime* time;
    void* value;
} TerraProfileBuffer;

typedef enum {
    U32,
    U64,
    I32,
    I64,
    F32,
    F64,
    TIME,
} TerraProfileSampleType;

typedef struct {
    // Sample type.
    TerraProfileSampleType type;
    // Global stats. They are computed putting together the sample buffers.
    TerraProfileStats stats;
    // One buffer per thread.
    TerraProfileBuffer* buffers;
} TerraProfileTarget;

typedef struct {
    size_t threads;
    uint32_t registered_threads;
    TerraProfileTarget* targets;
} TerraProfileSession;

typedef struct {
    TerraProfileSession* sessions;
} TerraProfileDatabase;

extern TerraProfileDatabase g_terra_profile_database;

void                terra_profile_session_create ( size_t id, size_t threads );
void                terra_profile_register_thread ( size_t session );
void                terra_profile_session_delete ( size_t id );

void                terra_profile_target_create_u32 ( size_t session, size_t target, size_t sample_cap );
void                terra_profile_target_create_u64 ( size_t session, size_t target, size_t sample_cap );
void                terra_profile_target_create_i32 ( size_t session, size_t target, size_t sample_cap );
void                terra_profile_target_create_i64 ( size_t session, size_t target, size_t sample_cap );
void                terra_profile_target_create_f32 ( size_t session, size_t target, size_t sample_cap );
void                terra_profile_target_create_f64 ( size_t session, size_t target, size_t sample_cap );
void                terra_profile_target_create_time ( size_t session, size_t target, size_t sample_cap );

void                terra_profile_target_clear ( size_t session, size_t target );
size_t              terra_profile_target_size ( size_t session, size_t target );
size_t              terra_profile_target_local_size ( size_t session, size_t target );
void                terra_profile_target_stats_update ( size_t session, size_t target );
void                terra_profile_target_local_stats_update ( size_t session, size_t target );
TerraProfileStats   terra_profile_target_stats_get ( size_t session, size_t target );
TerraProfileStats   terra_profile_target_local_stats_get ( size_t session, size_t target );
TerraProfileSampleType  terra_profile_target_type_get ( size_t session, size_t target );

void                terra_profile_add_sample_u32 ( size_t session, size_t target, uint32_t value );
void                terra_profile_add_sample_u64 ( size_t session, size_t target, uint64_t value );
void                terra_profile_add_sample_i32 ( size_t session, size_t target, int32_t value );
void                terra_profile_add_sample_i64 ( size_t session, size_t target, int64_t value );
void                terra_profile_add_sample_f32 ( size_t session, size_t target, float value );
void                terra_profile_add_sample_f64 ( size_t session, size_t target, double value );
void                terra_profile_add_sample_time ( size_t session, size_t target, TerraClockTime value );

// TODO
// operations on entire sessions (clear/update/size)
// an option to filter off the top/bottom parts of the samples before computing average/variance
// an option to select a time period on which to compute avg/var, instead of the whole buffer

// Use these macros to wrap profile calls so that when necessary they can be easily turned off without having to edit out code.
#define TERRA_PROFILE_REGISTER_THREAD( session )                            terra_profile_register_thread (session )
#define TERRA_PROFILE_CREATE_SESSION( session, threads )                    terra_profile_session_create ( session, threads )
#define TERRA_PROFILE_CREATE_TARGET( format, session, target, sample_cap)   terra_profile_target_create_ ## format ( session, target, sample_cap )
#define TERRA_PROFILE_ADD_SAMPLE( format, session, target, value)           terra_profile_add_sample_ ## format ( session, target, value )
#define TERRA_PROFILE_UPDATE_STATS( session, target )                       terra_profile_target_stats_update ( session, target )
#define TERRA_PROFILE_UPDATE_LOCAL_STATS( session, target )                 terra_profile_target_local_stats_update ( session, target )
#define TERRA_PROFILE_CLEAR_TARGET( session, target )                       terra_profile_target_clear ( session, target )
#define TERRA_PROFILE_DELETE_SESSION( session )                             terra_profile_session_delete ( session )
#define TERRA_CLOCK()                                                       terra_clock()

#else

#define TERRA_PROFILE_REGISTER_THREAD( session )                            0
#define TERRA_PROFILE_CREATE_SESSION( session, threads )                    0
#define TERRA_PROFILE_CREATE_TARGET( format, session, target, sample_cap)   0
#define TERRA_PROFILE_ADD_SAMPLE( format, session, target, value)           0
#define TERRA_PROFILE_UPDATE_STATS( session, target )                       0
#define TERRA_PROFILE_UPDATE_LOCAL_STATS( session, target )                 0
#define TERRA_PROFILE_CLEAR_TARGET( session, target )                       0
#define TERRA_PROFILE_DELETE_SESSION( session, target )                     0
#define TERRA_CLOCK()                                                       0

#endif

#ifdef __cplusplus
}
#endif