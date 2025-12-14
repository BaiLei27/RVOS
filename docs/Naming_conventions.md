## <font color =#f1c40f>命名规范</font>
<!-- + version: 24.5.6 -->
  + version: 25.11.16

#### <font color =#fc5531>文件夹命名</font>

+ 单个单词小写
    + `keyword`
    + 特殊文件夹首字母大写 `Modules`
+ 多个单词大驼峰(UpCamelCase)
    + `MyDoc`

#### <font color =#fc5531>文件命名</font>
+ C++ 文件以 *.cc 结尾(UpCamelCase)
    + 代码文件: `CmdSavePlayerInfoClass.cc` `MyUseFullClass.cc`
    + 头文件: `*.hh` `*.hpp`
    + 定义类的文件名一般是成对出现:
        + `FooBar.hh` `FooBar.cc`

+ C 文件 以 *.c 结尾(snake_case)
    + 代码文件: `my_src.c`
    + 头文件: `my_header.h`

+ 若是`*.h*`中含大量内联函数,追加后缀`_ini`
    +  `**_ini.h*`

### <font color =#457cc0>函数</font>
+ 常规函数每一个单词首字母大写
  + `int OpenFile(const string &f)` `bool IsFileName(const string &f)`

### <font color =#457cc0>常量</font>
+ 常量均需使用全部大写字母组成，并使用下划线分词
  + `const int APP_VER = 0024;`  //update current time

### <font color =#457cc0>参数、变量</font>
+ 变量名一律采用小驼峰式(camelCase)命名规则变量命名基本上遵循相应的英文表达或简写，在相对简单的环境（对象数量少、针对性强）中，可以将一些名称由完整单词简写为单个字母
+ 指针类型命名方式 前缀追加 `p`
    + `Base *pBase = new Base()` `int *pA = &a`
    +  常量指针 `const int *pINT = &a`


| user  | userId | index | display | buffer | argument | parameter | calculate | compare | column |
| :---: | :----: | :---: | :-----: | :----: | :------: | :-------: | :-------: | :-----: | :----: |
|   u   |  uId   |  idx  |  disp   |  buf   |   arg    |   param   |   calc    |   cmp   |  col   |


+ 简写参考网站
  + [allacronyms](https://www.allacronyms.com/)
  + [abbreviations](https://www.abbreviations.com/)


```c++
  int playerID;
  std::string tableName;
```

+ 若变量类型为 bool 类型，则名称应以 Has, Is, Can 或 en(enable) 开头

```c++
  bool isExist；
  bool hasConflict;
  bool canManage;
  bool enGitHook;
```
### <font color =#9642c1>全局变量</font>
+ 前缀标明一个变量的可见
  + 外部全局变量(global) `g_camelCase`
  + 静态全局变量(static global)`sg_camelCase`
  + 类、结构体、枚举 全局变量 `G_camelCase`
### <font color =#457cc0>类型命名规则</font>
+ 类结构体、枚举、类 每个单词首字母大写(UpCamelCase)
+ 成员变量
  + 函数
    + 公有(UpCamelCase)
    + 保护、私有(camelCase)
  + 数据: **追加后缀** `_`
    + 公有(camelCase_)
    + 保护、私有(UpCamelCase_)

```C++
// 抽象类命名
  class IBase {
  protected:
    virtual interface() =0;
  };

  class GamePlayerManager { //遵循内存对齐原则, 声明顺序自上而下从大到小
  public:
    const USER_GENDER_ // 目前常量无法从命名区分, 常量无法更改, 建议设为 public 权限(尽量不要使用常量成员)
    static inline s_playerHealth_;  //c++17

  protected:
    map<int, string> IntMapStr_; // 其他类数据成员与基本数据成员分开声明
    string Str_;

    int PlayerName_;
    const PLAYER_ID_;
    static s_PlayerInst_;
    static const s_PLAYER_POWERS_;

  private:
    int UserPassword_;
    static s_TimeCheck_;
  public: // 函数与数据 分开声明
    void GetUserInfo();
  private:
    void setUserInfo();
  };

  struct MyPlayerManager {
      //公有成员 默认只有 public 成员, 建议使用class
  };

  enum Week { //枚举值应该优先采用'常量'的命名方式
      OK = 0,
      OUT_OF_MEMORY = 1,
      MALFORMED_INPUT = 2,
  };
```
### <font color =#457cc0>名字空间命名(cpp)</font>
+ 尽量简化、缩写为一个单词 `namespace std` `namespace nw`(network)

### <font color =#457cc0>预处理</font>
+ 宏
  + 常量值:全大写加下划线 `#define MAX_LINE 32`
  + 单独宏定义以 单个下划线结尾`DEBUG_`(区别于标准库 `_MACRO` `_MACRO_`)
+ `using` 类型定义 追加后缀`_u`
  + `using u32_u= unsigned int`
+ `typedef` 类型定义 追加后缀`_t`
    + `typedef unsigned int u32_t`
    + `typedef` 定义数据结构(c)

```c
    typedef enum E_Colors_t {
      BLACK= 0,
    } E_Colors_t, pE_Colors_t*;

    typedef struct S_TextFormat_t {
      int structValue;
    } S_TextFormat_t, pS_TextFormat_t*;

    typedef union U_MyUnion_t {
      int unionValue;
    } U_MyUnion_t, pU_MyUnion_t*;
```
## <font color =#f1c40f>编码风格</font>

### <font color =#5ed67b>关于类构造初始化(C++)</font>
&emsp;想要严格的初始化列表和更严格的转换检查，推荐使用 '{ }'。
在处理基本类型或者希望使用默认构造函数进行初始化，可以使用 '( )'。
C++推荐使用 '{ }' 进行初始化，因为它提供了更好的一致性和类型安全性。在现代C++中，大多数代码都倾向于使用 '{ }' 进行初始化.

+ [参考1](https://zhuanlan.zhihu.com/p/268894227)
+ [参考2](https://zhuanlan.zhihu.com/p/693958568)
1.  在**不包含**`std::initializer_list` 构造器中尽量使用 `{ }`
2. `STL` 中 `{ }` 有不同含义, 使用 `( )`
3. 在使用标准库中的模板构造时尽量使用`( )`, 标准库中 对 `1` 中是否含`std::initializer_list` 不确定
4. 无参对象初始化时使用`{ }`
   + `MyClass obj();` 会产生歧义 声明一个函数`obj() -> MyClass `,`MyClass obj { }` 无歧义
   + 指针使用 `{ }` 初始化会默认初始化为 `nullptr`,智能指针不使用`{ }`,默认仍是  `nullptr`
---
<div align =right>
    <em>
        <strong>Create Date:2023/04/27<br>UpDate:25/11/16/23:23</strong>
    </em>
</div>


<!-- --- update:25/03/23/00:02 --- -->
