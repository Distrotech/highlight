/***************************************************************************
                          themereader.cpp  -  description
                             -------------------
    begin                : Son Nov 10 2002
    copyright            : (C) 2002-2010 by Andre Simon
    email                : andre.simon1@gmx.de
 ***************************************************************************/


/*
This file is part of Highlight.

Highlight is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Highlight is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Highlight.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "themereader.h"
#include <Diluculum/LuaState.hpp>

namespace highlight
{

ThemeReader::ThemeReader() : fileOK ( false )
{}

ThemeReader::~ThemeReader()
{
  for (unsigned int i=0;i<pluginChunks.size();i++){
      delete pluginChunks[i];
  }
}


void ThemeReader::initStyle(ElementStyle& style, const Diluculum::LuaVariable& var) {
    string styleColor="#000000";
    bool styleBold=false, styleItalic=false, styleUnderline=false;

    if (var["Colour"].value()!=Diluculum::Nil)
        styleColor= var["Colour"].value().asString();
    if (var["Bold"].value()!=Diluculum::Nil)
        styleBold= var["Bold"].value().asBoolean();
    if (var["Italic"].value()!=Diluculum::Nil)
        styleItalic= var["Italic"].value().asBoolean();
    if (var["Underline"].value()!=Diluculum::Nil)
        styleUnderline= var["Underline"].value().asBoolean();

    style.setColour(Colour(styleColor));
    style.setBold(styleBold);
    style.setItalic(styleItalic);
    style.setUnderline(styleUnderline);
}

bool ThemeReader::load ( const string &styleDefinitionPath )
{
    try {
        fileOK=true;
        Diluculum::LuaState ls;
        ls.doFile (styleDefinitionPath);

	desc = ls["Description"].value().asString();


	if (pluginChunks.size()){
	  Diluculum::LuaValueList params;
	  params.push_back(desc);
	  for (unsigned int i=0;i<pluginChunks.size();i++){
	    ls.call(*pluginChunks[i], params, "theme user function");
	  }
	}

        initStyle(canvas, ls["Canvas"]);
        initStyle(defaultElem, ls["Default"]);
        initStyle(comment, ls["BlockComment"]);
        initStyle(slcomment, ls["LineComment"]);
        initStyle(directive, ls["PreProcessor"]);
        initStyle(str, ls["String"]);
        initStyle(escapeChar, ls["Escape"]);
        initStyle(number, ls["Number"]);
        initStyle(dstr, ls["StringPreProc"]);
        initStyle(line, ls["LineNum"]);
        initStyle(operators, ls["Operator"]);

        int idx=1;
        ElementStyle kwStyle;
        char kwName[5];
        while (ls["Keywords"][idx].value() !=Diluculum::Nil) {
            initStyle(kwStyle, ls["Keywords"][idx]);
            snprintf(kwName, sizeof(kwName), "kw%c", ('a'+idx-1)); // TODO kwa -> kw1...
            keywordStyles.insert ( make_pair ( string(kwName),kwStyle ));
            idx++;
        }
        
        if (ls.globals().count("Injection")){
	    injection = ls["Injection"].value().asString();
	}
    } catch (Diluculum::LuaFileError err) {
        errorMsg = string(err.what());
        return fileOK=false;
    } catch (Diluculum::TypeMismatchError err) {
        errorMsg = string(err.what());//expected "+err.getExpectedType()+ ", got "+err.getFoundType();
        return fileOK=false;
    } catch (Diluculum::LuaSyntaxError err) {
        errorMsg = "syntax error: "+string(err.what());
        return fileOK=false;
    }

    return fileOK;
}

string ThemeReader::getErrorMessage() const
{
    return errorMsg;
}

Colour ThemeReader::getBgColour() const
{
    return canvas.getColour();
}

ElementStyle ThemeReader::getDefaultStyle() const
{
    return defaultElem;
}

ElementStyle ThemeReader::getCommentStyle() const
{
    return comment;
}

ElementStyle ThemeReader::getSingleLineCommentStyle() const
{
    return slcomment;
}

ElementStyle ThemeReader::getStringStyle() const
{
    return str;
}

ElementStyle ThemeReader::getPreProcStringStyle() const
{
    return dstr;
}

ElementStyle ThemeReader::getEscapeCharStyle() const
{
    return escapeChar;
}

ElementStyle ThemeReader::getNumberStyle() const
{
    return number;
}

ElementStyle ThemeReader::getPreProcessorStyle() const
{
    return directive;
}

ElementStyle ThemeReader::getLineStyle() const
{
    return line;
}

ElementStyle ThemeReader::getOperatorStyle() const
{
    return operators;
}

bool ThemeReader::found () const
{
    return fileOK;
}

ElementStyle ThemeReader::getKeywordStyle ( const string &className )
{
    if ( !keywordStyles.count ( className ) ) return defaultElem;
    return keywordStyles[className];
}

vector <string> ThemeReader::getClassNames() const
{
    vector <string> kwClassNames;
    for ( KSIterator iter = keywordStyles.begin(); iter != keywordStyles.end(); iter++ )
    {
        kwClassNames.push_back ( ( *iter ).first );
    }
    return kwClassNames;
}

KeywordStyles ThemeReader::getKeywordStyles() const
{
    return keywordStyles;
}

}
