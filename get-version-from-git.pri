# 版本号构成 MAJAR_NUMBER.MINOR_NUMBER.CHANGE_NUMBER.BUILD_NUMBER
message( **************[START] reading $$TARGET version **************)

MAJAR_NUMBER = 0
MINOR_NUMBER = 0
CHANGE_NUMBER = 0
BUILD_NUMBER = 0

# 从git分支名中，读取 大、中、小版本。若当前分支不是发布分支，将尝试从最近git标签中读取。
exists(.git) {

    #git提交数作为构建版本号。
    BUILD_NUMBER = $$system(git rev-list HEAD --count)
    message(build number  = $$BUILD_NUMBER)

    #git分支名称
    GIT_BRANCH = $$system(git symbolic-ref --short -q HEAD)
    message(git branch = $$GIT_BRANCH)
    GIT_BRANCH_PREFIX = $$str_member($$GIT_BRANCH,0,7)
    equals(GIT_BRANCH_PREFIX,release/){
        VERSION_NUMBER_STRING = $$replace(GIT_BRANCH,[^0-9+|\.],)
        VERSION_NUMBER_LIST = $$split(VERSION_NUMBER_STRING,.)
        VERSION_NUMBER_LIST_SIZE = $$size(VERSION_NUMBER_LIST)
    }else{
        message("current git branch is not release branch, release branch name should start withs 'release/'.")
    }

    lessThan(VERSION_NUMBER_LIST_SIZE,3){
        #分支名不符合 release/v0.0.0 尝试从最近的tag中读取
        message(try to read version from nearest tag.)
        GIT_NEAREST_TAG = $$system(git describe --tags --match v[0-9]*.[0-9]*.[0-9]* --abbrev=0)
        message(git nearest tag = $$GIT_NEAREST_TAG)
        VERSION_NUMBER_STRING = $$replace(GIT_NEAREST_TAG,[^0-9+|\.],)
        VERSION_NUMBER_LIST = $$split(VERSION_NUMBER_STRING,.)
        VERSION_NUMBER_LIST_SIZE = $$size(VERSION_NUMBER_LIST)
    }
    greaterThan(VERSION_NUMBER_LIST_SIZE,2){
        MAJAR_NUMBER = $$member(VERSION_NUMBER_LIST,0)
        MINOR_NUMBER = $$member(VERSION_NUMBER_LIST,1)
        CHANGE_NUMBER = $$member(VERSION_NUMBER_LIST,2)
        VERSION = $$MAJAR_NUMBER.$$MINOR_NUMBER.$$CHANGE_NUMBER.$$BUILD_NUMBER
    }else{
        message(can not read version from git!)
    }
} else {
    message(can not read version from git!)
}
isEmpty(VERSION){
    exists(version.pri){
        include(version.pri)
        message("include version.pri")
    }else{
        message("try to include version.pri, but version.pri is not exists.")
    }
}
isEmpty(VERSION){
    message(can not determine version of $$TARGET)
} else {
    message(version = $$VERSION)
}
message(***************[END] reading $$TARGET version***************)
