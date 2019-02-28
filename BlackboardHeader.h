#ifndef BLACKBOARDHEADER_H
#define BLACKBOARDHEADER_H
    #ifdef NSB_BLACKBOARD_DEV
        #define NSB_BLACKBOARD_EXPORT
    #else
        #ifdef NSB_BLACKBOARD_SHARED
            #define NSB_BLACKBOARD_EXPORT Q_DECL_EXPORT
        #else
            #define NSB_BLACKBOARD_EXPORT Q_DECL_IMPORT
        #endif
    #endif
#endif // BLACKBOARDHEADER_H
