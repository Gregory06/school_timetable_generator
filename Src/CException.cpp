//
// Created by Gregory Postnikov on 2019-07-20.
//

#include "CException.h"

CException::CException(std::string msg)
    : msg_(msg)
    {}

std::string CException::GetMessage() const {
    return msg_;
}

CBadSubjectPlacement::CBadSubjectPlacement( std::string msg,
                                            const CSubject* subject )
                                            : CException(msg),
                                            subject_(subject)
                                            {}

CBadCabinetsFind::CBadCabinetsFind( std::string msg,
                                    const CSubject* subject )
                                    : CException(msg),
                                    subject_(subject)
                                    {}

const CSubject* CBadSubjectPlacement::GetSubject() {
    return subject_;
}

const CSubject* CBadCabinetsFind::GetSubject() {
    return subject_;
}