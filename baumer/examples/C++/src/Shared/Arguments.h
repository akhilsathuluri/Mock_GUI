// Arguments.h
#ifndef SHARED_OPTIONS_H_
#define SHARED_OPTIONS_H_

enum ArgumentMode {
    eArgumentInit = 0,
    eArgumentAdd = 1,
    eArgumentRemove = 2,
};

struct Argument {
    void* const pData;
    const char* pShortName;
    const char* pName;
    bool bFlag;
    void(*pProcessArgument)(const Argument& argument, const ArgumentMode mode, const char* const pParam);
    unsigned int value;
    const char* pDescription1;
    const char* pDescription2;
};

/** Standard initialisation and parsing function for boolean argument */
extern void argumentBoolFlag(const Argument& argument, const ArgumentMode mode, const char* pParam);

/** Standard initialisation and parsing function for unsigned integer parameter */
extern void argumentUint(const Argument& argument, const ArgumentMode mode, const char* const pParam);

/** Standard initialisation and parsing function for std::string argument */
extern void argumentString(const Argument& argument, const ArgumentMode mode, const char* const pParam);

/** Parsing of boolean argument - default: "On" = true / "Off" = false. On/Off state strings can be user supplied.
    result is true if pParam successfully parsed, false elsewhere. */
extern bool isBoolParam(const char* const pParam, bool& bValue, const char* const pParamOn = NULL, const char* const pParamOff = NULL);

/** Parses all command line arguments and/or an optional 'Config.txt' configuration file */
extern void parseArguments(const Argument* const pArgumentList, const unsigned int argumentCount, int argc, char* argv[]);

#endif  // SHARED_OPTIONS_H_
