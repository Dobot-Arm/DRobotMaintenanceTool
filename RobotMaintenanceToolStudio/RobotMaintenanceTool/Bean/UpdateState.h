#ifndef UPDATESTATE_H
#define UPDATESTATE_H

enum UpdateState
{
    E_FAILED = -1, //失败
    E_READY = 0, //准备升级
    E_UPDATING = 1, //升级中
    E_SUCCESS = 2, //升级成功
};

#endif // UPDATESTATE_H
