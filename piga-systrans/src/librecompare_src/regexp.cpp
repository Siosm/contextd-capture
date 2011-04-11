#include <QRegExp>
#include "regexp.h"

RegExp::RegExp()
{
}

RegExp::RegExp(int /*epsilon*/)
{
	setEpsilon();
}

RegExp::RegExp(const QString &pattern) :
		_pattern(pattern)
{
	_transitions=parsePattern(pattern, true);
}

RegExp::RegExp(const RegExp &rx) :
		_pattern(rx.pattern()),
		_transitions(rx.transitions())
{
}

RegExp::~RegExp()
{

}

inline void RegExp::setPattern(const QString &pattern)
{
	_pattern=pattern;
	_transitions=parsePattern(pattern, true);
}

inline void RegExp::setEpsilon()
{
	_transitions.clear();

	Transition t;
	t.setEpsilon();

	_transitions.append(t);
	_pattern=QString::fromUtf8("ε");
}

inline bool RegExp::isEmpty() const
{
	return _transitions.isEmpty() || _transitions.first().token.type==EpsilonType;
}

inline bool RegExp::isEpsilon() const
{
	if(_transitions.isEmpty())
		return false;

	const Transition &t=_transitions.first();

	if(t.token.type==EpsilonType)
		return true;
	else if(t.token.type==SubExprType)
		return t.token.sub_exprs[0]->isEpsilon();
	else if(t.token.type==LogicalOrType)
	{
		for(int i=0; i<t.token.sub_exprs.size(); ++i)
			if(!t.token.sub_exprs.at(i)->isEpsilon())
				return false;

		return true;
	}
	else
		return false;
}

inline bool RegExp::isNull() const
{
	return _transitions.isEmpty();
}

inline QString RegExp::pattern() const
{
	return _pattern;
}

inline QList<RegExp::Transition> RegExp::transitions() const
{
	return _transitions;
}

static inline RegExp::Transition takeFirstTransition(QList<RegExp::Transition> &list)
{
	if(list.isEmpty())
	{
		RegExp::Transition t;
		t.setEpsilon();
		return t;
	}
	else
		return list.takeFirst();
}

static inline RegExp::Transition takeLastTransition(QList<RegExp::Transition> &list)
{
	if(list.isEmpty())
	{
		RegExp::Transition t;
		t.setEpsilon();
		return t;
	}
	else
		return list.takeLast();
}

static inline QString patternExtractStart(QString &pattern, const RegExp::Transition &t)
{
	QString extracted;

	//Default lens for token and operator strings
	int tokenLen=0, opLen=0;

	//Calculate the len of the token in the pattern string
	if(t.token.type==RegExp::NormalType)
		tokenLen=pattern[0]==RegExp::EscapeSymbol ? 2:1;
	else if(t.token.type==RegExp::AnyType)
		tokenLen=1;
	else if(t.token.type==RegExp::BracketType || t.token.type==RegExp::ExcludeType)
		tokenLen=pattern.indexOf(QRegExp("[^\\\\]\\]"))+2;
	else if(t.token.type==RegExp::SubExprType)
	{
		//Find ')', remove everything after
		for(int j=1, depth=0; j<pattern.size() && tokenLen==0; ++j)
		{
			if(pattern[j]==RegExp::SubExprBegin && pattern[j-1]!=RegExp::EscapeSymbol)
				++depth;
			else if(pattern[j]==RegExp::SubExprEnd && pattern[j-1]!=RegExp::EscapeSymbol)
			{
				if(depth)
					--depth;
				else
					tokenLen=j+1;
			}
		}
	}
	else if(t.token.type==RegExp::LogicalOrType)
		tokenLen=pattern.size();
	else if(t.token.type==RegExp::EpsilonType)
		tokenLen=QString::fromUtf8("ε").length();

	//Calculate the len of the operator in the pattern string
	if(t.op==RegExp::OneOrMoreOp || t.op==RegExp::OneOrZeroOp || t.op==RegExp::KleeneStarOp)
		opLen=1;

	//Remove the current transition from pattern so that it's ready for the next operations
	extracted=pattern.left(tokenLen);
	pattern.remove(0, tokenLen+opLen);

	return extracted;
}

static inline QString patternExtractEnd(QString &pattern, const RegExp::Transition &t)
{
	QString extracted;

	//Default lens for token and operator strings
	int tokenLen=0, opLen=0;

	//Calculate the len of the operator in the pattern string
	if(t.op==RegExp::OneOrMoreOp || t.op==RegExp::OneOrZeroOp || t.op==RegExp::KleeneStarOp)
		opLen=1;

	//Calculate the len of the token in the pattern string
	if(t.token.type==RegExp::NormalType)
	{
		if(pattern.size()<=1+opLen || pattern[pattern.size()-2-opLen]!=RegExp::EscapeSymbol)
			tokenLen=1;
		else
			tokenLen=2;
	}
	else if (t.token.type==RegExp::AnyType)
		tokenLen=1;
	else if(t.token.type==RegExp::BracketType || t.token.type==RegExp::ExcludeType)
	{
		//Find the '[' of the good depth
		for(int j=pattern.size()-2-opLen; j>0 && tokenLen==0; --j)
		{
			if(pattern[j]==RegExp::BracketBegin && pattern[j-1]!=RegExp::EscapeSymbol)
				tokenLen=pattern.size()-j-opLen;
		}

		//If the '[' was at index 0 and thus not found above
		if(tokenLen==0)
			tokenLen=pattern.size()-opLen;
	}
	else if(t.token.type==RegExp::SubExprType)
	{
		//Find the '(' of the good depth
		for(int j=pattern.size()-2-opLen, depth=0; j>0 && tokenLen==0; --j)
		{
			if(pattern[j]==RegExp::SubExprEnd && pattern[j-1]!=RegExp::EscapeSymbol)
				++depth;
			else if(pattern[j]==RegExp::SubExprBegin&& pattern[j-1]!=RegExp::EscapeSymbol)
			{
				if(depth)
					--depth;
				else
					tokenLen=pattern.size()-j-opLen;
			}
		}

		//If the '(' was at index 0 and thus not found above
		if(tokenLen==0)
			tokenLen=pattern.size()-opLen;
	}
	else if(t.token.type==RegExp::LogicalOrType)
		tokenLen=pattern.size();
	else if(t.token.type==RegExp::EpsilonType)
		tokenLen=QString::fromUtf8("ε").length();

	//Remove the current transition from pattern so that it's ready for the next operations
	extracted=pattern.right(tokenLen+opLen);
	extracted.chop(opLen);
	pattern.chop(tokenLen+opLen);

	return extracted;
}

static inline void appendRegExp(RegExp &a, const RegExp &b)
{
	a.append(b);
}

static inline void prependRegExp(RegExp &a, const RegExp &b)
{
	a.prepend(b);
}

QVector<Derivation> RegExp::derivations(TakeNextTransitionFn next, PatternExtractFn extract, RegExpConcatFn expr_append) const
{
	QVector<Derivation> deriv, existingDeriv;

	//Expression is ø, left-derivatives are all ø
	if(_transitions.isEmpty())
		return deriv;

	//Build the derivates till a non-nullable transition is encountered
	QList<Transition> newTrans=_transitions;
	QString newPattern=pattern(), oldPattern;
	bool nullable, epsilon;

	do
	{
		//Fetch the next transition and check it's nullability
		Transition t=next(newTrans);
		nullable=t.isNullable();
		epsilon=t.isEpsilon();

		//If there is no next transition, no need to calculate token derivations
		if(!epsilon)
		{
			oldPattern=extract(newPattern, t);

			//Fetch the left derivations for the current transition's token
			QVector<Derivation> tokenDeriv=t.token.derivations(next, extract, expr_append);
			QVector<Derivation>::iterator it;

			//Build the RegExp matching the rest of the expression (will be appended to the token's derivations)
			RegExp newExpr(newPattern, newTrans);

			//For ConcatOp or OneOrZeroOp, just append newExpr to the token's derivations so that they are complete
			//Since OneOrZeroOp is nullable, the other derivations of the expression will be built in the next loop iteration
			if(t.op==ConcatOp || t.op==OneOrZeroOp)
			{
				for(it=tokenDeriv.begin(); it!=tokenDeriv.end(); ++it)
					expr_append((*it).expr, newExpr);
			}
			//For KleeneStarOp and OneOrMoreOp, the derivation will contain the same token with a Kleene star (making a loop)
			//Since KleeneStarOp is nullable, other derivations will be built for the expression in the next loop iteration
			else if(t.op==OneOrMoreOp || t.op==KleeneStarOp)
			{
				t.op=KleeneStarOp;
				QList<Transition> oldTrans;
				oldTrans << t;
				RegExp kleeneExpr(oldPattern+KleeneStar, oldTrans);
				expr_append(kleeneExpr, newExpr);

				for(it=tokenDeriv.begin(); it!=tokenDeriv.end(); ++it)
					expr_append((*it).expr, kleeneExpr);
			}

			//Merge the found token derivations with the existing ones in order to preserve determinability of the derivations
			for(int i=0; i<tokenDeriv.size(); ++i)
			{
				Derivation d=tokenDeriv.at(i);

				existingDeriv=deriv;
				deriv.clear();

				if(d.token.type==BracketType)
				{
					while(!existingDeriv.isEmpty() && !d.isEmpty())
					{
						Derivation d2=existingDeriv.last();
						existingDeriv.pop_back();

						//If both derivations have common characters, create a third one recognizing both expressions
						if(d2.token.type==BracketType)
						{
							ByteSet inter=d.token.data & d2.token.data;
							if(!inter.isEmpty())
							{
								//These transitions must not contain characters from inter anymore to preserve determinability
								d.token.data-=inter;
								d2.token.data-=inter;

								//Only contains characters from d2, so will not conflict with deriv's other items
								Derivation d3(DerivationToken(BracketType, inter), d.expr|d2.expr);

								deriv << d3;
							}
						}

						//Characters in d and not in d2 also recognize d2's expression
						else if(d2.token.type==ExcludeType)
						{
							ByteSet inter=d.token.data & d2.token.data;

							//Contains characters recognizing both expressions, and will not conflict since d2 wasn't conflicting
							Derivation d3(DerivationToken(BracketType, d.token.data-inter), d.expr|d2.expr);

							//d1 and d2 can not match characters from d3 to preserve determinability
							d.token.data=inter;
							d2.token.data+=d3.token.data;

							if(!d3.isEmpty())
								deriv << d3;
						}

						//d also recognizes the expression from d2, as long as d has one character that matches
						else if(d2.token.type==AnyType)
						{
							//This will not impact previously modified derivations since they were already compatible with d2
							d.expr|=d2.expr;

							//Since d contains a finite set of characters, d2 must exclude these characters
							d2.token=DerivationToken(ExcludeType, d.token.data);
						}

						if(!d2.isEmpty())
							deriv << d2;
					}
				}

				else if(d.token.type==ExcludeType)
				{
					while(!existingDeriv.isEmpty() && !d.isEmpty())
					{
						Derivation d2=existingDeriv.last();
						existingDeriv.pop_back();

						//Characters in d2 and not in d also recognize d's expression
						if(d2.token.type==BracketType)
						{
							ByteSet inter=d.token.data & d2.token.data;

							//Contains characters recognizing both expressions, and will not conflict since d2 wasn't conflicting
							Derivation d3(DerivationToken(BracketType, d2.token.data-inter), d.expr|d2.expr);

							//d1 and d2 can not match characters from d3 to preserve determinability
							d.token.data+=d3.token.data;
							d2.token.data=inter;

							if(!d3.isEmpty())
								deriv << d3;
						}

						//d and d2 both have a list of excluded characters
						else if(d2.token.type==ExcludeType)
						{
							//A derivation for the characters recognized by both expressions
							Derivation d3(DerivationToken(ExcludeType, d.token.data|d2.token.data), d.expr|d2.expr);

							//A derivation for the characters excluded in d2, which might conflict and should be appended later
							Derivation d4(DerivationToken(BracketType, d2.token.data-d.token.data), d.expr);

							//Characters excluded only in d should match the expression from d2
							d2.token.type=BracketType;
							d2.token.data=d.token.data-d2.token.data;

							//Make sure that the original derivation is not appended since it was replaced by d4
							d.token.data.clear();

							deriv << d3;

							if(!d4.isEmpty())
								tokenDeriv << d4;
						}

						//d also recognizes the expression from d2, as long as d has one character that matches
						else if(d2.token.type==AnyType)
						{
							//This will not impact previously modified derivations since they were already compatible with d2
							d.expr|=d2.expr;

							//Since d matches all characters but a finite set, d2 can recognize only those missing characters
							d2.token=DerivationToken(BracketType, d.token.data);
						}

						if(!d2.isEmpty())
							deriv << d2;
					}
				}
				//Derivation matches any character at all
				else if(d.token.type==AnyType)
				{
					//Fetch the first existing derivation and use it to turn d into a more little derivation
					if(!existingDeriv.isEmpty())
					{
						Derivation d2=existingDeriv.last();
						existingDeriv.pop_back();

						//Turn d into an Exclude derivation of the chars in d2, and make d2 match d's expression
						if(d2.token.type==BracketType)
						{
							d2.expr|=d.expr;
							Derivation d4(DerivationToken(ExcludeType, d2.token.data), d.expr);
							tokenDeriv << d4;
						}
						//Turn d into a Bracket derivation of the chars in d2, and make d2 match d's expression
						else if(d2.token.type==ExcludeType)
						{
							d2.expr|=d.expr;
							Derivation d4(DerivationToken(BracketType, d2.token.data), d.expr);
							tokenDeriv << d4;
						}
						//Just make d2 match both expressions, and don't append d
						else if(d2.token.type==AnyType)
						{
							d2.expr|=d.expr;
						}

						if(!d2.isEmpty())
							deriv << d2;

						//This will make sure d is not appended, since d4 will replace it for further merging where needed
						d.token.type=NoType;
						d.token.data.clear();
					}
				}

				//If d is still to be appended, append it
				if(!d.isEmpty())
					deriv << d;
				//Else restore the derivations that were not merged with d yet but that were valid
				else
				{
					deriv << existingDeriv;
				}
			}
		}
	} while(nullable && !epsilon);

	return deriv;
}

QVector<Derivation> RegExp::leftDerivations() const
{
	return derivations(takeFirstTransition, patternExtractStart, appendRegExp);
}

QVector<Derivation> RegExp::rightDerivations() const
{
	return derivations(takeLastTransition, patternExtractEnd, prependRegExp);
}

inline bool RegExp::isNullable() const
{
	return isNullable(0);
}

bool RegExp::operator <(const RegExp &other) const
{
	return pattern() < other.pattern();
}

RegExp::RegExp(const QString &pattern, const QList<Transition> &transitions) :
		_pattern(pattern),
		_transitions(transitions)
{
}

QList<RegExp::Transition> RegExp::parsePattern(const QString &pattern, bool top_level) const
{
	QList<Transition> transitions;

	//Abort if the pattern is not syntaxically valid
	QRegExp expr(pattern);
	if(!expr.isValid())
		return transitions;

	//The current transition will be used for storing the last found tokens by waiting for the next operator
	Transition current;
	current.token.type=NoType;
	current.op=ConcatOp;

	//Flags for knowing what has been found. Only append transitions when both are true
	bool foundToken=false;
	bool foundOp=false;

	//Whether the next token should be considered a normal char
	bool escaped;

	//The data to parse, in a 1-byte encoding
	QByteArray data=pattern.toLatin1();

	for(int i=0; i<data.size(); ++i)
	{
		char token=data.at(i);
		escaped=false;

		//If the character is escaped, go to next character
		if(token==EscapeSymbol)
		{
			if(i!=data.size()-1)
			{
				++i;
				token=data.at(i);
				escaped=true;
			}
			else
				qWarning("RegExp::parsePattern(): '%c' can not be the last character", EscapeSymbol);
		}

#ifndef NDEBUG
		//Expression beginning character
		if(token==ExpressionBegin && !escaped)
		{
			if(i!=0 || !top_level)
				qWarning("RegExp::parsePattern(): '%c' is only allowed as the first character", ExpressionBegin);
		}
		else if(token==ExpressionEnd &&  !escaped)
		{
			if(i!=data.size()-1 || !top_level)
				qWarning("RegExp::parsePattern(): '%c' is only allowed as the last character", ExpressionEnd);
		}
		else
#endif

		//If the token is an Operator
		if((token==KleeneStar||token==OneOrMore||token==OneOrZero) && !escaped)
		{
			// ** +* ?* or * give *
			if(token==KleeneStar)
				current.op=KleeneStarOp;
			else if(token==OneOrMore)
			{
				// ?+ or *+ give *
				if(foundOp && current.op!=OneOrMoreOp)
					current.op=KleeneStarOp;
				// ++ or + give +
				else
					current.op=OneOrMoreOp;
			}
			else if(token==OneOrZero)
			{
				// +? or *? give *
				if(foundOp && current.op!=OneOrZeroOp)
					current.op=KleeneStarOp;
				// ?? or ? give ?
				else
					current.op=OneOrZeroOp;
			}

			foundOp=true;
		}

		//If the token is a Token
		else
		{
			//If there is already a found token, add a transition
			if(foundToken)
			{
				//The default operator is the concatenation
				if(!foundOp)
					current.op=ConcatOp;

				//Append the previously found token
				transitions.append(current);
				current.token.data.clear();
				current.token.sub_exprs.clear();
				foundToken=false;
				foundOp=false;
			}

			if(token==SubExprBegin &&  !escaped)
			{
				QByteArray subPattern;
				current.token.type=SubExprType;

				//Find ')', copy everything before it, and everything after '('
				for(int j=i+1, depth=0; j<data.size() && subPattern.isEmpty(); ++j)
				{
					if(data[j]==SubExprBegin && data[j-1]!=EscapeSymbol)
						++depth;
					else if(data[j]==SubExprEnd && data[j-1]!=EscapeSymbol)
					{
						if(depth)
							--depth;
						else
							subPattern=data.mid(i+1, j-(i+1));
					}
				}

				//Remove the characters between brackets from the data so that they're not parsed afterwards
				//closingIndex represents the number of chars between the brackets, so add 2 to remove the brackets too
				data.remove(i, subPattern.size()+2);

				//Decrement i since the next character to parse is now at index i
				i--;

				current.token.sub_exprs.append(QSharedPointer<RegExp>(new RegExp(QString(subPattern))));
			}
			else if(token==LogicalOr && !escaped)
			{
				//RegExp matching the next part of the pattern
				QString after(data.right(data.size()-i-1));
				RegExp next(after);

				//RegExp matching what has already been parsed
				data.truncate(i);
				RegExp prev(data, transitions);

				//Use operator | to properly sort both RegExps
				RegExp ordered(prev|next);

				//Current transition is the first transition of ordered (which only has one anyway)
				current=ordered.transitions().first();

				//Clear the transitions and let the algorithm terminate (so that it appends (current, ConcatOp) to transitions)
				transitions.clear();
			}
			else if((token==BracketBegin && !escaped) ||
					(escaped && (QString("sSwWdD").contains(token))))
			{
				QString bracketData;

				//If the token is a special character, initialise the bracket data to the matching hardcoded value
				//Else, scan the pattern for a ']' character
				switch(token)
				{
				case 's':
					bracketData=" \t\r\n\v\f";
					break;
				case 'S':
					bracketData="^ \t\r\n\v\f";
					break;
				case 'd':
					bracketData="0-9";
					break;
				case 'D':
					bracketData="^0-9";
					break;
				case 'w':
					bracketData="A-Za-z0-9_";
					break;
				case 'W':
					bracketData="^A-Za-z0-9_";
					break;
				}

				//Case '[...]'
				if(bracketData.isEmpty())
				{
					//Copy the data, remove everything before '['
					bracketData=data.right(data.size()-i-1);

					//Find ']', remove everything after
					int closingIndex=bracketData.indexOf(QRegExp("[^\\\\]\\]"))+1;
					bracketData.truncate(closingIndex);

					//Remove the characters between brackets from the data so that they're not parsed afterwards
					//closingIndex represents the number of chars between the brackets, so add 2 to remove the brackets too
					data.remove(i, closingIndex+2);

					//Decrement i since the next character to parse is now at index i
					i--;
				}

				//Calculate ranges for each found '-' that is not escaped and surrounded by other chars
				int index, from=0;
				while((index=bracketData.indexOf(QRegExp("[^\\\\]-."), from)) != -1)
				{
					ByteSet expanded;

					//Destroy any '--' sequence
					while(bracketData[index+2]==Range)
						bracketData.remove(index+2, 1);

					//Remove any escape character before the first character of the range
					if(index-1>=0)
						if(bracketData[index-1]==EscapeSymbol)
							bracketData.remove(--index, 1);

					//Remove any escape character before the second character of the range
					if(index+3<bracketData.size())
						if(bracketData[index+2]==EscapeSymbol)
							bracketData.remove(index+2, 1);

					//Turn the '-' into whatever is between those two
					char rangeBegin=bracketData[index].toLatin1()+1;
					char rangeEnd=bracketData[index+2].toLatin1();
					for(char c=rangeBegin; c<rangeEnd; ++c)
						expanded+=c;

					bracketData.replace(index+1, 1, expanded);

					//Begin searching after the appended characters for new '-' occurrences
					from=index+1+expanded.size();
				}

				//Set the good token type depending on the presence of the exclude character
				if(bracketData[0]==Exclude)
				{
					current.token.type=ExcludeType;
					//Remove leading '^'
					bracketData.remove(0, 1);
				}
				else
					current.token.type=BracketType;

				current.token.data=bracketData.toLatin1();
			}

#ifndef NDEBUG
			//End of sub-expression or bracket - should have been parsed somewhere else and not been caught
			else if(token==SubExprEnd && !escaped)
				qWarning("RegExp::parsePattern(): found unexpected '%c'", SubExprEnd);
			else if(token==BracketEnd && !escaped)
				qWarning("RegExp::parsePattern(): found unexpected '%c'", BracketEnd);
#endif

			else if(token==AnyCharacter && !escaped)
				current.token.type=AnyType;
			else
			{
				current.token.type=NormalType;
				current.token.data.resize(1);
				current.token.data[0]=token;
			}

			foundToken=true;
		}
	}

	//Append the last token, if any, without operator if none was found at the end
	if(foundToken)
	{
		if(!foundOp)
			current.op=ConcatOp;

		transitions.append(current);
		current.token.data.clear();
		current.token.sub_exprs.clear();
	}

	return transitions;
}

bool RegExp::isNullable(int index) const
{
	//Expression is ø
	if(_transitions.size()<=index)
		return false;
	else
	{
		const Transition &t=_transitions.at(index);

		//Operator of the transition makes it nullable
		if(t.op==KleeneStarOp || t.op==OneOrZeroOp)
		{
			if(_transitions.size()==index+1)
				return true;
			else
				return isNullable(index+1);
		}

		//Calculate nullable for the current token
		//It is false for normal characters, *, [] and [^]
		bool nullableToken=false;

		//Token is ε
		if(t.token.type==EpsilonType)
			nullableToken=true;
		//Token is a sub-expression
		else if(t.token.type==SubExprType)
			nullableToken=t.token.sub_exprs.at(0)->isNullable();
		//Token is a OR-combination of sub-expressions
		else if(t.token.type==LogicalOrType)
		{
			for(int i=0; i<t.token.sub_exprs.size() && !nullableToken; ++i)
				nullableToken |= t.token.sub_exprs.at(i)->isNullable();
		}

		//If this is the last item of the list, return nullableToken
		if(_transitions.size()==index+1)
			return nullableToken;
		//Else, we have either OneOrMoreOp or ConcatOp, so we need to also check the next transition's nullability
		else
			return nullableToken && isNullable(index+1);
	}
}

RegExp &RegExp::prepend(const RegExp &b)
{
	//If the regexp is empty, assign the other one instead
	if(isEmpty() && !b.isNull())
		return this->operator =(b);

	//If the other regexp is empty, nothing to do
	if(b.isEmpty())
		return *this;

	//Fetch the first transition
	Transition t=_transitions.first();

	//Fetch the other expression's last transition
	QList<Transition> bTransitions=b.transitions();
	QString bPattern=b.pattern();
	Transition bt=bTransitions.last();

	//If the current expression is based on a logical OR
	if(t.token.type==LogicalOrType)
	{
		//If the other expression is also a OR
		if(bt.token.type==LogicalOrType)
			return this->operator |=(b);
		//Surround the current expression with parenthesis before prepending b
		else
		{
			t.token.type=SubExprType;
			t.token.sub_exprs << QSharedPointer<RegExp>(new RegExp(*this));
			t.op=ConcatOp;
			_pattern=SubExprBegin+_pattern+SubExprEnd;

			//Replace the first transition since changes have been done
			_transitions.takeFirst();
			_transitions << t;
		}
	}
	//If the other expression is a OR, surround it with parenthesis
	else if(bt.token.type==LogicalOrType)
	{
		bt.token.type=SubExprType;
		bt.token.sub_exprs << QSharedPointer<RegExp>(new RegExp(b));
		bt.op=ConcatOp;
		bPattern=SubExprBegin+bPattern+SubExprEnd;

		//Replace the last transition of the other expr since changes have been done
		bTransitions.takeLast();
		bTransitions << t;
	}

	//Prepend the other expression's pattern and transitions
	_pattern.prepend(b.pattern());
	while(!bTransitions.empty())
		_transitions.prepend(bTransitions.takeLast());

	return *this;
}



RegExp &RegExp::append(const RegExp &b)
{
	//If the regexp is empty, assign the other one instead
	if(isEmpty() && !b.isNull())
		return this->operator =(b);

	//If the other regexp is empty, nothing to do
	if(b.isEmpty())
		return *this;

	//Fetch the last transition
	Transition t=_transitions.last();

	//Fetch the other expression's first transition
	QList<Transition> bTransitions=b.transitions();
	QString bPattern=b.pattern();
	Transition bt=bTransitions.first();

	//If the current expression is based on a logical OR
	if(t.token.type==LogicalOrType)
	{
		//If the other expression is also a OR
		if(bt.token.type==LogicalOrType)
			return this->operator |=(b);
		//Surround the current expression with parenthesis before prepending b
		else
		{
			t.token.type=SubExprType;
			t.token.sub_exprs << QSharedPointer<RegExp>(new RegExp(*this));
			t.op=ConcatOp;
			_pattern=SubExprBegin+_pattern+SubExprEnd;

			//Replace the last transition since changes have been done
			_transitions.takeLast();
			_transitions << t;
		}
	}
	//If the other expression is a OR, surround it with parenthesis
	else if(bt.token.type==LogicalOrType)
	{
		bt.token.type=SubExprType;
		bt.token.sub_exprs << QSharedPointer<RegExp>(new RegExp(b));
		bt.op=ConcatOp;
		bPattern=SubExprBegin+bPattern+SubExprEnd;

		//Replace the first transition of the other expr since changes have been done
		bTransitions.takeFirst();
		bTransitions << t;
	}

	//Append the other expression's pattern and transitions
	_pattern+=bPattern;
	_transitions << bTransitions;

	return *this;
}

RegExp &RegExp::operator +=(const RegExp& b)
{
	return append(b);
}

static bool subExpressionListSort(QSharedPointer<RegExp> a, QSharedPointer<RegExp> b)
{
	return *a < *b;
}

RegExp &RegExp::operator |=(const RegExp& b)
{
	//If the regexp is null, assign the other one instead
	if(isNull())
		return this->operator =(b);

	//If the added RegExp is null, abort
	else if(b.isNull())
		return *this;

	//If the current RegExp is ε
	else if(isEpsilon())
	{
		//ε|ε gives ε, so there is no change to do
		if(b.isEpsilon())
			return *this;
		//ε|nullable gives nullable, so assign b
		else if(b.isNullable())
			return this->operator =(b);
	}
	//nullable|ε gives nullable, so there is no change to do
	else if(b.isEpsilon() && isNullable())
		return *this;

	//General case: X|Y
	QList<QSharedPointer<RegExp> > subExprs;

	//Get the sub expressions for this expression
	if(_transitions.first().token.type==LogicalOrType)
		subExprs << _transitions.first().token.sub_exprs;
	else
		subExprs << QSharedPointer<RegExp>(new RegExp(*this));

	//Get the sub expressions for the other expression
	if(b._transitions.first().token.type==LogicalOrType)
		subExprs << b._transitions.first().token.sub_exprs;
	else
		subExprs << QSharedPointer<RegExp>(new RegExp(b));

	//Sort expressions to allow pattern comparison of expressions
	qSort(subExprs.begin(), subExprs.end(), subExpressionListSort);

	//Remove duplicates to avoid creating expressions such as (X|X|X...) - this ensures terminaison in Automaton's constructor
	QList<QSharedPointer<RegExp> >::iterator it=subExprs.begin();
	QString prevPattern="";
	while(it!=subExprs.end())
	{
		const QSharedPointer<RegExp> &ptr=*it;

		if(ptr->pattern()==prevPattern)
			it=subExprs.erase(it);
		else
		{
			prevPattern=ptr->pattern();
			++it;
		}
	}

	//Create a logical OR transition t
	Transition t;
	t.token.type=LogicalOrType;
	t.op=ConcatOp;
	t.token.sub_exprs=subExprs;

	//Update pattern
	_pattern="";
	for(QList<QSharedPointer<RegExp> >::const_iterator it=subExprs.begin(); it!=subExprs.end(); ++it)
	{
		_pattern+=(*it)->pattern()+LogicalOr;
	}
	_pattern.chop(1);

	//Set t as the only transition of the expression
	_transitions.clear();
	_transitions << t;

	return *this;
}

RegExp operator |(const RegExp &a, const RegExp &b)
{
	RegExp r(a);
	r|=b;
	return r;
}

bool operator ==(const RegExp &a, const RegExp &b)
{
	return a.pattern()==b.pattern();
}

RegExp &RegExp::operator =(RegExp b)
{
	_pattern=b.pattern();
	_transitions.clear();
	_transitions=b.transitions();

	return *this;
}

QVector<Derivation> RegExp::Token::derivations(TakeNextTransitionFn next, PatternExtractFn extract, RegExpConcatFn expr_append) const
{
	QVector<Derivation> deriv;

	//If the token represents a sub-expression, return all its left derivations
	if(type==RegExp::SubExprType)
		return sub_exprs.first()->derivations(next, extract, expr_append);
	if(type==RegExp::LogicalOrType)
	{
		for(int i=0; i<sub_exprs.size(); ++i)
			deriv << sub_exprs.at(i)->derivations(next, extract, expr_append);
		return deriv;
	}

	//Else, build a derivation made of this token followed by the ε regexp
	RegExp::DerivationToken tok(*this);

	//Convert NormalType to BracketType
	if(type==RegExp::NormalType)
		tok.type=RegExp::BracketType;

	//Remove extra '\0' if needed
	if(tok.data[tok.data.size()]=='\0')
		tok.data.resize(tok.data.size());

	deriv.append(Derivation(tok, RegExp(1)));
	return deriv;
}

RegExp::DerivationToken::DerivationToken()
{
}

RegExp::DerivationToken::DerivationToken(const RegExp::Token &token) :
		type(token.type),
		data(token.data)
{
}

RegExp::DerivationToken::DerivationToken(const TokenType &type, const ByteSet &data) :
		type(type),
		data(data)
{
}

inline void RegExp::Transition::setEpsilon()
{
	token.type=EpsilonType;
	op=ConcatOp;
}

inline bool RegExp::Transition::isEpsilon() const
{
	return token.type==EpsilonType;
}

bool RegExp::Transition::isNullable() const
{
	//Expression can contain ε
	if(op==KleeneStarOp || op==OneOrZeroOp)
		return true;

	//Calculate nullable for the current token
	//It is false for normal characters, *, [] and [^]
	bool nullableToken=false;

	//Token is ε
	if(token.type==EpsilonType)
		nullableToken=true;
	//Token is a sub-expression
	else if(token.type==SubExprType)
		nullableToken=token.sub_exprs.at(0)->isNullable();
	//Token is a OR-combination of sub-expressions
	else if(token.type==LogicalOrType)
	{
		for(int i=0; i<token.sub_exprs.size() && !nullableToken; ++i)
			nullableToken |= token.sub_exprs.at(i)->isNullable();
	}

	//We don't care about the next transition, we are only interested in the nullability of the transition
	return nullableToken;
}

Derivation::Derivation()
{
}

Derivation::Derivation(const RegExp::DerivationToken &token, const RegExp &expr) :
		token(token),
		expr(expr)
{
}

bool Derivation::isEmpty() const
{
	return token.type!=RegExp::AnyType && token.data.isEmpty();
}
