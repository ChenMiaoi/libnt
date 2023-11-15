<!-- 
@author: Chen Miao   
@date: 2023/11/15  
-->

# 开发规范文档

主要用于团队内开发准则，使得项目具有可读性和低耦合。  

## 目录  

- [开发规范文档](#开发规范文档)
  - [目录](#目录)
  - [1. Overview](#1-overview)
  - [2. Git提交办法](#2-git提交办法)
  - [3. 变量](#3-变量)
    - [3.1. 普通变量](#31-普通变量)
    - [3.2. 常量](#32-常量)
    - [3.3. 成员变量](#33-成员变量)
    - [3.4. static](#34-static)
  - [4. 注释](#4-注释)
  - [5. 函数](#5-函数)
  - [6. 类](#6-类)
  - [7. 接口/抽象](#7-接口抽象)
  - [8. 更多细化](#8-更多细化)
    - [8.1 NULL \& nullptr](#81-null--nullptr)
    - [8.2 type traits](#82-type-traits)
    - [8.3 lambda](#83-lambda)

---  

##  1. <a name='Overview'></a>Overview

| 作用对象 | 规则 | 作用对象 | 规则 |  
| :---: | :--- | :---: | :--- |  
| 变量 | 区分普通变量和成员变量以及常量 | 函数 | 区分普通函数和成员函数 |  
| 类 | 知名见意，主要依照标准库 | 接口/抽象 | 知名见意 |
| 各种细化规则 | ... | ... | ... |  

---  

##  2. <a name='Git'></a>Git提交办法

> 针对git进行管理代码版本的提交方法

| 等级 | 解释 | 等级 | 解释 |  
| :---: | :--- | :---: | :--- |  
| feat | 创建新文件、功能等 | update | 更新现有文件、功能等 |  
| fix | 修复以往代码产生的错误 | docs | 用于更新或添加文档 |  
| test | 用于添加、修改和测试用例 | format | 修改格式(作为保留字) |  

```bash
> git commit -m "feat: create a new feature"
> git commit -m "update: add x to xX feature"
> git commit -m "fix: what causes the bug"
> git commit -m "docs: create a xxx document"
> git commit -m "test: ..."
```

- 注意：**该提交办法适用于自己拉取的feature分支或dev分支，*严禁一切在main分支上的修改和提交***。  

---

##  3. <a name='Variable'></a>变量

> 针对于变量来说，主要分为`普通变量`、`成员变量`、`常量`以及`static`

###  3.1. <a name='GeneralVariable'></a>普通变量

1. 对于`普通变量`的实行方法主要是，以**蛇形命名方法**，可以搭配缩写，但尽量知名见意(情况实在不允许的情况下，请写全含义)。  
2. **必须初始化**，严禁出现非初始化的情况发生(**特例，`container`可以使用`{}`直接初始化**)

``` c++
/*** 规范一 ***/
int this_is_num = 0;
std::string name = "miao"; // 禁止随意使用auto a, b, c...

/*** 规范二 ***/
int number = 0;
// int number; [err] 语法上是合理的，但不符合开发规范
std::string name {};
// std::string name; [err] 语法上是合理的，但不符合开发规范
```

###  3.2. <a name='ConstVariable'></a>常量

1. 对于`常量`的实行方法主要是，以**大写命名方法，用下划线间隔**，可以搭配缩写，但尽量知名见意。  
2. 如果`常量`是在针对`普通变量`、`container`、`函数`、`成员函数`等，则**按照各自的行为规范进行实行**  
3. 尽可能的使用`constexpr const`来取代`#define`的写法  

> 值得注意的是：**因为定义常量就证明在后续一定会有用，因此此处就不在强调必须初始化，但是未初始化依旧不符合规范**

``` c++
/*** 规范一 ***/
const int MAX_SIZE = 10; 
// [err] const int MAXSIZE
// [err] const int MAX_SIZE;

/*** 规范二 ***/
const std::string& func() {}

/*** 规范三 ***/
constexpr const int MAX_NUM = 128;
#define MAX_NUM 128 // [warn] 这两种写法是完全等效的，使用上一种来取代宏的大量使用  
```  

###  3.3. <a name='MemberVariable'></a>成员变量 

1. **禁止出现公开成员变量的情况**(因此你需要在一开始设计的时候就想好具体的接口和需要的属性)  
2. 如若需要公开`成员变量`，那么你应该选择使用`struct`而非`class`  
3. **私有成员采用下划线 + 命名的方式**(具体命令规范依照`普通变量`和`常量`准则)    
4. 针对`getter`和`setter`的设计，需要做出严格限制   

``` c++
/*** 规范一 ***/
class Person {
public:
    std::string name;  // [err] 严禁直接出现公有成员
    std::string _name; // [ok]
}

/*** 规范三 ***/
class Person {
    std::string _name;
public:
    const std::string& get_name() { return _name; }     // [ok]
    void   set_name(const std::string& name) {          // [ok]
        if (name.empty()) {
            print("set name must be a non-Null!");
        }
        _name = name;
    }
}
```

###  3.4. <a name='Static'></a>static

1. 对于`static`的实行办法是，如果`static`没有外加`const`，那么采用一般变量规则，否则采用常量规则。
2. **严格注意`static`变量作为成员变量**  
3. **需要注意，在`static`中，可以选择不初始化**

``` c++
/*** 规范一 ***/
static int this_num = 0;
static int n; // [ok]  

static const int MAX_SIZE = 10;

/*** 规范二 ***/
class Person {
    static std::string _info; // [warn] 作为成员变量
}
std::string _info = "init";
```

---  

##  4. <a name='Comment'></a>注释

为了一般化管理，注释也需要做出特定办法。  

1. 每一个文件开头必须做出文档注释，以`///`开头。
2. 每一个函数和类必须做出文档注释，以`/***/`开头，并且，内容包括`@author`, `@date`, `@brief`(也就是对应的作者、日期、和简要),**在必要情况下，请加上`@param`、`@return`**，也就是`Doxygen`风格注释
3. 对于变量的注释，采取有必要则注释的方法，以`//`开头即可

``` c++
/*** 规范一 ***/
// in Person.dart

/// This is .....
/// ....

/*** 规范二 ***/
/**
 * @author Miao
 * @date 2023/10/02
 * @brief for string comparing
 * @param src the source string
 * @param dst the destination string
 * @return if src < dst, return -1, if src > dst, return 1, other return 0
 */
int strcmp(const std::string& src, const std::string& dst);

/*** 规范三 ***/
int num = 0; // comment
```

---  

##  5. <a name='Function'></a>函数

对于函数，我们有以下实行办法：

1. 函数名必须知名见意，且风格以**蛇形命名**为主。  
2. 必须加上必要的函数注释规范  
3. 在非特殊情况下，函数尽量能够适应于多种接口，以提高重用。  
4. 如果函数十分简单，且内部无修改，尽量使用`const`和`constexpr`以实现`编译期计算`  
5. 非必要，不要加上`static`，除非是在类中作为静态函数  

``` c++
/*** 规范一 ***/
void fromJson(std::map<T>);  // [err] 不符合规范一
void from_json(std::map<T>); 

/*** 规范二 ***/
/**
 * @author Miao
 * @date 2023/10/02
 * @brief use Json format to init a object
 */
void from_json(std::map<T>);

/*** 规范三 ***/
Person.from_json(...);
Cat.from_json(...);

print_json(String);
print_json(num);
...

/*** 规范四 ***/
constexpr int get_num() const {
    return _num;
}

/*** 规范五 ***/
Point::distance(Point, Point); // [ok]
```  

---  

##  6. <a name='Class'></a>类  

对于类，我们采用以下办法：

1. 类名知名见意，以**大驼峰**命名风格  
2. 加上必要的注释  
3. **类尽量单一文件，严禁其他外部代码直接访问类中成员**
4. 严格区分`operator`、`构造函数`、以及成员变量和成员函数
5. **如果是`单参数构造`，那么必须加上`explicit`防止`隐式类型转换`**  

``` c++
/*** 规范一 ***/
class PersonInfo;

/*** 规范三 ***/
// in PersonInfo.dart
class PersonInfo {  
    int    _age;
    std::string _name;
    std::string _addr;
public:
    /*** 规范四 ***/
    PersonInfo();
    explicit PersonInfo(int age);
    ...
    PersonInfo(int age, const std::string& name, const std::string& addr);

    PersonInfo& operator+ (PersonInfo); 
    PersonInfo& operator- (PersonInfo);
    ...
    PersonInfo operator...
}

int get_age() {
    return PersonInfo()._age;           // [err] 严格禁止
}
```

---

##  7. <a name='AbstractInterface'></a>接口/抽象

对于接口与抽象的实行办法：

1. 命名风格依照类规范一实行
2. 尽可能的使得接口和抽象满足大部分需求
3. 不会导致过高的耦合度

---  

##  8. <a name='More'></a>更多细化  

### 8.1 <a name='NULL'></a>NULL & nullptr  

在`C++`中，为了防止`NULL`二义性的存在，使用了一种新类型`nullptr_t`。  

**因此，我们决定禁止`NULL`的使用出现，所有需要使用`NULL`的地方，都将会替换为`nullptr`**    

``` c++
const char* name = nullptr; // not NULL
```

### 8.2 <a name='type traits'></a>type traits  

在类中，我们会使用到很多的原生类型，例如`int`、`char*`等，为了开发中的可读性和可维护性，需要尽可能的对类型进行修饰。  

因此，**我们决定在类中尽可能使用`type traits`来使得可读性更好**  

``` c++
class Person {
    using __self     = Person*;
    using __self_ref = Person&;
    ...
} ;
```

### 8.3 <a name='lambda'></a>lambda  

在`C++11`以后，引入了`lambda`的机制。基于这种情况，**我们决定，函数的传递和封装使用`functional` + `lambda`的方式。  


``` c++
std::function<void*(void)> func = []() -> void* {
    return nullptr;
};
```