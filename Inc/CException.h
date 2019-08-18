//
// Created by Gregory Postnikov on 2019-07-20.
//

#ifndef TIMER_CEXCEPTION_H
#define TIMER_CEXCEPTION_H

class CException : virtual public std::exception {
public:
    explicit CException(std::string msg);

    std::string GetMessage() const;

private:
    std::string msg_;
};

class CBadSubjectPlacement : public CException {
    const CSubject* subject_;

public:
    CBadSubjectPlacement(std::string msg, const CSubject* subject);

    const CSubject* GetSubject();
};

class CBadCabinetsFind : public CException {
    const CSubject* subject_;

public:
    CBadCabinetsFind(std::string msg, const CSubject* subject);

    const CSubject* GetSubject();
};

class CBadTimeTable : public CException {
public:
    explicit CBadTimeTable(std::string msg)
    : CException(msg)
    {}
};

#endif //TIMER_CEXCEPTION_H
