/*
 * Copyright (c) 2003-2004  Pau Arumí & David García
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <MiniCppUnit.hxx>

TestsListener& TestsListener::theInstance()
{
	static TestsListener instancia;
	return instancia;
}

std::stringstream& TestsListener::errorsLog()
{
	if (_currentTestName)
		_log << "\n" << errmsgTag_nameOfTest() << (*_currentTestName) << "\n";
	return _log;
}

std::string TestsListener::logString()
{
	std::string aRetornar = _log.str();
	_log.str("");
	return aRetornar;
}
void TestsListener::currentTestName( std::string& name)
{
	_currentTestName = &name;
}
void TestsListener::testHasRun()
{
	std::cout << ".";
	theInstance()._executed++;
}
void TestsListener::testHasFailed()
{
	std::cout << "F";
	theInstance()._failed++;
}
void TestsListener::testHasThrown()
{
	std::cout << "E";
	theInstance()._exceptions++;
}
std::string TestsListener::summary()
{
	std::ostringstream os;
	os	<< "\nSummari:\n"
		<< "\033[" "1m\tExecuted Tests:         " 
		<< _executed << "\033[0m\n"
		<< "\033[32;1m\tPassed Tests:           " 
		<< (_executed-_failed-_exceptions) << "\033[0m\n";
	if (_failed > 0)
	{
		os 	<< "\033[31;1m\tFailed Tests:           " 
			<< _failed << "\033[0m\n";
	}
	if (_exceptions > 0)
	{
		os 	<< "\033[31;1m\tUnexpected exceptions: " 
			<< _exceptions << "\033[0m\n";
	}
	os << std::endl;
	return os.str();
}
bool TestsListener::allTestsPassed()
{
	return !theInstance()._exceptions && !theInstance()._failed;
}




void Assert::assertTrue(char* strExpression, bool expression,
		const char* file, int linia)
{
	if (!expression)
	{
		TestsListener::theInstance().errorsLog() << "\n"
			<< errmsgTag_testFailedIn() << file 
			<< errmsgTag_inLine() << linia << "\n" 
			<< errmsgTag_failedExpression() << strExpression << "\n";
		TestsListener::theInstance().testHasFailed();
	}
}

void Assert::assertTrueMissatge(char* strExpression, bool expression, 
		const char* missatge, const char* file, int linia)
{
	if (!expression)
	{
		TestsListener::theInstance().errorsLog() << "\n"
			<< errmsgTag_testFailedIn() << file
			<< errmsgTag_inLine() << linia << "\n" 
			<< errmsgTag_failedExpression() << strExpression << "\n"
			<< missatge<<"\n";
		TestsListener::theInstance().testHasFailed();
	}
}


/*
void Assert::assertIguals( const char * expected, const char * result,
	const char* file, int linia )
{
	assertIguals(std::string(expected), std::string(result),
		file, linia);
}
void Assert::assertIguals( const bool& expected, const bool& result,
	const char* file, int linia )
{
	assertIguals(
		(expected?"true":"false"), 
		(result?"true":"false"),
		file, linia);
}
*/
/**
 * we overload the assert with string doing colored diffs
 *
 * MS Visual6 doesn't allow string by reference :-( 
 */
 /*
void Assert::assertIguals( const std::string expected, const std::string result,
	const char* file, int linia )
{
	if(expected == result)
		return;
	
	int indexDiferent = notEqualIndex(expected, result);
	TestsListener::theInstance().errorsLog()
		<< file << ", linia: " << linia << "\n"
		<< errmsgTag_expected() << "\n\033[36;1m" 
		<< expected.substr(0,indexDiferent)
		<< "\033[32;1m" << expected.substr(indexDiferent) << "\033[0m\n"
		<< errmsgTag_butWas() << "\033[36;1m \n" << result.substr(0,indexDiferent)
		<< "\033[31;1m" << result.substr(indexDiferent) << "\033[0m\n";

	TestsListener::theInstance().testHasFailed();
}
*/
int Assert::notEqualIndex( const std::string & one, const std::string & other )
{
	int end = std::min(one.length(), other.length());
	for ( int index = 0; index < end; index++ )
		if (one[index] != other[index] )
			return index;
	return end;
}

void Assert::fail(const char* motiu, const char* file, int linia)
{
	TestsListener::theInstance().errorsLog() <<
		file << errmsgTag_inLine() << linia << "\n" <<
		"Reason: " << motiu << "\n";

	TestsListener::theInstance().testHasFailed();
}


