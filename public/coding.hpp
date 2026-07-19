extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <iostream>
#include <string>

std::string base64_encode(const std::string &in) {
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_write(bio, in.c_str(), in.length());
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);

  std::string encoded(bufferPtr->data, bufferPtr->length);
  BIO_free_all(bio);

  return encoded;
}

std::string base64_decode(const std::string &in) {
  BIO *bio, *b64;
  int decodeLength = in.length();
  char *decode = new char[decodeLength];

  bio = BIO_new_mem_buf(in.data(), -1);
  b64 = BIO_new(BIO_f_base64());
  bio = BIO_push(b64, bio);

  decodeLength = BIO_read(bio, decode, decodeLength);
  BIO_free_all(bio);

  std::string decoded(decode, decodeLength);
  delete[] decode;

  return decoded;
}

// Lua函数封装
int l_base64_encode(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  std::string encoded = base64_encode(str);
  lua_pushstring(L, encoded.c_str());
  return 1;  // 返回一个值（编码后的字符串）
}

int l_base64_decode(lua_State *L) {
  const char *str = luaL_checkstring(L, 1);
  std::string decoded = base64_decode(str);
  lua_pushstring(L, decoded.c_str());
  return 1;  // 返回一个值（解码后的字符串）
}

void lua_test() {
  lua_State *L = luaL_newstate();  // 创建Lua状态机
  luaL_openlibs(L);                // 加载Lua基础库

  if (luaL_dostring(L, "print('Hello from Lua!')") != LUA_OK) {
    std::cerr << lua_tostring(L, -1) << std::endl;  // 输出错误信息
  }

  // 将C++ 函数注册为Lua函数
  lua_register(L, "base64_encode", l_base64_encode);
  lua_register(L, "base64_decode", l_base64_decode);

  // 测试
  // 载入Lua函数并放在堆栈顶部
  lua_getglobal(L, "base64_encode");

  // 将参数放在堆栈上
  lua_pushstring(L, "Hello, World!");

  // 调用Lua函数，1个参数，1个返回值
  if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
    std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);  // 弹出错误消息
  } else {
    // 获取结果并打印
    const char *encoded_result = lua_tostring(L, -1);
    std::cout << "Encoded in C++: " << encoded_result << std::endl;

    // 弹出返回值，保持堆栈干净
    lua_pop(L, 1);
  }

  lua_close(L);  // 关闭Lua状态机}
}
