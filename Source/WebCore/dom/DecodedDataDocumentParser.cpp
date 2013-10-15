/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "DecodedDataDocumentParser.h"

#include "DocumentWriter.h"
#include "SegmentedString.h"
#include "TextResourceDecoder.h"

#include <wtf/text/CString.h>



using namespace std;
QTM_USE_NAMESPACE

#define MAX_ERROR_MSG 0x1000

namespace WebCore {

DecodedDataDocumentParser::DecodedDataDocumentParser(Document* document)
    : DocumentParser(document)
{
}


int DecodedDataDocumentParser::compileRegex (regex_t * r, const char * regex_text)
{
    int status = regcomp (r, regex_text, REG_EXTENDED|REG_NEWLINE);
    if (status != 0) {
        char error_message[MAX_ERROR_MSG];
        regerror (status, r, error_message, MAX_ERROR_MSG);
        printf ("Regex error compiling '%s': %s\n",
             regex_text, error_message);
        return 1;
    }
    return 0;
}



QContact DecodedDataDocumentParser::getContact(string name, string surname) { 

	QContactFetchRequest fetchRequest; 
	QContactSortOrder s;  
	s.setDirection(Qt::DescendingOrder);  
	s.setCaseSensitivity(Qt::CaseInsensitive);  
	s.setDetailDefinitionName("Name","FirstName");  
	s.setBlankPolicy(QContactSortOrder::BlanksFirst);  
	QList<QContactSortOrder> sortList;
	sortList.push_back(s); 
 
	//Search filters  
	QContactUnionFilter nameFilter; 
	QContactDetailFilter subFilterF;  
	subFilterF.setDetailDefinitionName("Name", "FirstName");  
	subFilterF.setValue(name.c_str());  
	subFilterF.setMatchFlags(QContactFilter::MatchContains);  
	nameFilter.append(subFilterF); 
	QContactDetailFilter subFilterL;  
	subFilterL.setDetailDefinitionName("Name", "LastName");  
	subFilterL.setValue(surname.c_str());  
	subFilterL.setMatchFlags(QContactFilter::MatchContains);  
	nameFilter.append(subFilterL); 
 
	fetchRequest.setSorting(sortList);  
	fetchRequest.setFilter(nameFilter);  
	fetchRequest.setManager(new QContactManager(&fetchRequest)); 
	//connect(request, SIGNAL(stateChanged(QContactAbstractRequest::State)),  //SLOT(onStateChanged(QContactAbstractRequest::State)) 
	fetchRequest.start(); 
	
	QEventLoop loop;
	QObject::connect(&fetchRequest, SIGNAL(stateChanged(QContactAbstractRequest::State)), &loop, SLOT(quit()));
	loop.exec();

	QList<QContact> list=fetchRequest.contacts();  
	qWarning("Contacts readed %d",list.count()); 

	if(list.count()>0) {   
		return list.at(0);  
	} 
	else {   
		QContact c;
		return c;    
	} 

} 


string DecodedDataDocumentParser::replaceSocamPersonTags (string text)
{

	regex_t regex;
	const char* tag_regex_text = "<\\s*person\\s*>([^,]*),([^<]*)<\\s*/person\\s*>";
	
	compileRegex(&regex, tag_regex_text);	
	size_t ngroups = regex.re_nsub + 1;
	regmatch_t *groups = (regmatch_t *) malloc(ngroups * sizeof(regmatch_t));

	//string text(to_match);

	while (1) {

		const char * p = text.c_str();
		int nomatch = regexec(&regex, p, ngroups, groups, 0);

		if (nomatch) {
	   		break;
		}

		regmatch_t match = groups[0];
		if (groups[0].rm_so == -1) {
			break;
		}

		string name, surname;
		size_t nmatched;
		for (nmatched = 0; nmatched < ngroups; nmatched++) {

			if (groups[nmatched].rm_so == (size_t)(-1)) {
				break;
			}

			string matchStr = text.substr (groups[nmatched].rm_so,groups[nmatched].rm_eo-groups[nmatched].rm_so);
			cout << "Match: "<< matchStr << endl;
			if(nmatched == 1) {
				name = matchStr;
			}
			else if(nmatched == 2) {
				surname = matchStr;
			}
			
		}

		cout << "Parsed: name=" << name << " surname=" << surname << endl;


		string tag = text.substr (match.rm_so,match.rm_eo-match.rm_so);

		QContact contact = getContact(name, surname);
		
		//qWarning("\tContact ID: %d -- UUID: %s",contact.localId(),contact.details("Guid").at(0).value("Guid").toStdString().c_str());
		//qWarning("\t\tDisplayLabel: %s",contact.details("DisplayLabel").at(0).value("Label").toStdString().c_str());
		//qWarning("\t\tEmail %s",contact.details("EmailAddress").at(0).value("EmailAddress").toStdString().c_str());
		//qWarning("\t\tPhoneNumber: %s",contact.details("PhoneNumber").at(0).value("PhoneNumber").toStdString().c_str());
		/*
		QContactDetail(name="DisplayLabel", key=7, "Label"=QVariant(QString, "Self Simulator") );
		QContactDetail(name="Type", key=10, "Type"=QVariant(QString, "Contact") );
		QContactDetail(name="Name", key=13, "CustomLabel"=QVariant(QString, "Self Simulator") , "FirstName"=QVariant(QString, "Self") , "LastName"=QVariant(QString, "Simulator") );
		QContactDetail(name="EmailAddress", key=16, "EmailAddress"=QVariant(QString, "selfcontact@example.com") );
		QContactDetail(name="PhoneNumber", key=19, "PhoneNumber"=QVariant(QString, "+44123456789") );
		QContactDetail(name="Address", key=22, "Country"=QVariant(QString, "UK") , "Locality"=QVariant(QString, "Leister") , "Street"=QVariant(QString, "56 Edmonton Square") );
		QContactDetail(name="Guid", key=25, "Guid"=QVariant(QString, "0") );
		QContactDetail(name="Timestamp", key=28, "CreationTimestamp"=QVariant(QDateTime, QDateTime("dom 13. mar 12:21:09 2011") ) , "ModificationTimestamp"=QVariant(QDateTime, QDateTime("dom 13. mar 12:21:09 2011") ) );
		*/
		
		ostringstream oss ;

		oss << "<table>";

		oss << "<tr>";
		if(contact.details("DisplayLabel").count()>0) {
			oss << "<td>" << contact.details("DisplayLabel").at(0).value("Label").toStdString() << "</td>";
		}
		oss << "</tr>";

		oss << "<tr>";
		if(contact.details("EmailAddress").count()>0) {
			oss << "<td>" << contact.details("EmailAddress").at(0).value("EmailAddress").toStdString() << "</td>";
		}
		oss << "</tr>";

		oss << "<tr>";
		if(contact.details("PhoneNumber").count()>0) {
			oss << "<td>" << contact.details("PhoneNumber").at(0).value("PhoneNumber").toStdString() << "</td>";
		}
		oss << "</tr>";

		oss << "</table>";

		string newTagContent = oss.str();

		text.replace(match.rm_so,match.rm_eo-match.rm_so,newTagContent);
		cout<<"Replaced:" <<text<<endl;

	}

	regfree (&regex);
	return text;

}





string DecodedDataDocumentParser::replaceSocamAccelerometerTags (string text)
{

	regex_t regex;
	const char* tag_regex_text = "<\\s*accelerometer\\s*/>";
	
	compileRegex(&regex, tag_regex_text);	
	regmatch_t* groups = new regmatch_t[1];

	//string text(to_match);

	
	while (1) {

		const char * p = text.c_str();
		int nomatch = regexec(&regex, p, 1, groups, 0);

		if (nomatch) {
	   		break;
		}

		regmatch_t match = groups[0];
		if (groups[0].rm_so == -1) {
			break;
		}
		
		string tag = text.substr (match.rm_so,match.rm_eo-match.rm_so);
		cout << "Matched(" << match.rm_so << "," << match.rm_eo << "): " << tag << endl;

		SOCAMAPI::SocamAccelerometerApi api;
		api.refreshCoord();
		int x = api.getCoord(SOCAMAPI::COORD_X);
		int y = api.getCoord(SOCAMAPI::COORD_Y);
		int z = api.getCoord(SOCAMAPI::COORD_Z);
		std::ostringstream oss;
		oss << "<div class='S_AC_DIV'> <table class='S_AC_TABLE'>" 
			<< ""
			<< "<tr><td>" << x << "</td><td>" << y << "</td><td>" << z 
			<< "</td></tr></table></div>";
		string newTagContent = oss.str();

		text.replace(match.rm_so,match.rm_eo-match.rm_so,newTagContent);
		cout<<"Replaced:" <<text<<endl;

	}

	regfree (&regex);
	return text;

}



void DecodedDataDocumentParser::appendBytes(DocumentWriter* writer , const char* data, int length, bool shouldFlush)
{
    std::cout<<"DecodedDataDocumentParser::appendBytes!!!!!!!: "<< ( data!=NULL ? strlen(data) : 0) <<std::endl;

    string dataString;

    if (data!=NULL) {

	dataString = data;
        dataString = replaceSocamPersonTags(dataString);
        dataString = replaceSocamAccelerometerTags(dataString);
        //cout<<"Result:"<<replacedData.c_str()<<endl;
	//cout<<endl<<endl<<"REPLACED DATA:"<<data<<endl<<endl;

    }

    if (!length && !shouldFlush)
        return;

    TextResourceDecoder* decoder = writer->createDecoderIfNeeded();
    String decoded = decoder->decode(dataString.c_str(), dataString.size());
    if (shouldFlush)
        decoded += decoder->flush();

    cout<<"Decoded:"<<decoded.ascii().data()<<endl;

    if (decoded.isEmpty())
        return;

    writer->reportDataReceived();

    append(decoded);
}







};

