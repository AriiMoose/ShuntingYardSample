#include <stdio.h>
#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

/* ====================================================================================
	Description:	- Shunting-Yard algorithm is used to parse the expression & change it to
						Reverse Polish Notation (RPN)/Postfix
					 - Evaluated result will be calculated using RPN
					 - Shunting-Yard was chosen as it is a proven algorithm for this use case;
						additionally in a production environment the Shunting-Yard can easily be
						understood by coworkers and applied to other languages thanks to it being
						a "standard" proven algorithm. Furthermore, in a production environment
						the Shunting-Yard is easily extended to include adjustable operator priority,
						additional operators (functions, exponentials etc), and decimal values
					 - Vectors used for dynamic storage
					 - Arithmetic expressions are tokenised before being shunted, and finally
						calculated using RPN
					 - Pass-By-Reference used over Pass-By-Value for efficiency, and convenience
						when modifying result sets

	Author:			Andrew Tully
	Last Modified:	8th March 2017
=======================================================================================*/

// Method declarations
bool isOperator(const std::string& token);
bool verifyInteger(const std::string& token);
bool isParenthesis(const std::string& token);
bool evaluate(const std::string& expression, int &result);

#pragma region Global Methods
/*	Function:	Verify if a given token is an arithmetic operator
Parameters:	1) String ref - token
Returns:	bool
*/
bool isOperator(const std::string& token)
{
	return token == "+" || token == "-" ||
		token == "*" || token == "/";
}

/*	Function:	Verify if a given token is a bracket
Parameters:	1) String ref - token
Returns:	bool
*/
bool isParenthesis(const std::string& token)
{
	return token == "(" || token == ")";
}

/*	Function:	Verify that a given token is an integer
Parameters:	1) String ref - token
Returns:	bool
*/
bool verifyInteger(const std::string& token)
{
	// Flag used to indiciate if the token is an integer
	bool isInteger = false;

	// Iterate over each element of the token
	// **Important since numerical tokens may have multiple digits
	for (unsigned int i = 0; i < token.size(); i++)
	{
		// Check if the current element is a digit or not
		// A flag is set instead of simply returning true
		// This accounts for tokens being multiple digits
		// If any element of the token is not a digit we return false immediately
		// Otherwise, the flag is set to true rather than returning true in case
		//		the following elements are non-digits
		if (!isdigit(token[i]))
		{
			return false;
		}

		else
		{
			isInteger = true;
		}
	}

	return isInteger;
}
#pragma endregion

#pragma region Shunting Yard Algorithm
class ShuntingYard
{
	// Public declarations
	public:
		bool shuntInfixToRPN(const std::vector<std::string>& inputTokens, std::vector<std::string>& returnArray);

	private:
		// Declare operator stack
		// Stores arithmetic operators
		std::stack<std::string> operatorStack;

		// Declare output queue
		// Stores RPN output
		std::queue<std::string> outputQueue;
};

/*	Function:	- Implementation of the Shunting-Yard algorithm
				- Convert infix notation to postfix (RPN)
	Parameters: 1) Vector<string> ref inputTokens	- Reference to the tokens to convert
				2) Vector<string ref returnArray	- Reference to the collection of postfix tokens
													- Pass-by-reference used to directly modify the
														stored collection without having to return a value & 
														allow the method to return true/false for a successful conversion
	Returns: 1) bool	- Whether or not the conversion was successful
*/
bool ShuntingYard::shuntInfixToRPN(const std::vector<std::string>& inputTokens, std::vector<std::string>& returnArray)
{
	// Iterate through tokens
	for (unsigned int i = 0; i < inputTokens.size(); i++)
	{
		const std::string currentToken = inputTokens[i];				// Cache the current token to check
		
		// Check if token is a number
		if (verifyInteger(currentToken))
		{
			// Push number to output queue
			outputQueue.push(currentToken);
		}

		// Check if token is an operator
		else if (isOperator(currentToken))
		{
			// Push to operator stack
			operatorStack.push(currentToken);
		}

		// Check if token is left bracket
		else if (currentToken == "(")
		{
			// Push to operator stack
			operatorStack.push(currentToken);
		}

		// Check if token is right bracket
		else if (currentToken == ")")
		{
			// Cache the initial top token of the operator stack
			std::string topOperatorStackToken = operatorStack.top();

			// Until the token at the top of the stack is a left parenthesis, 
			//		pop operators off the stack onto the output queue
			// When the loop exits either the stack will be empty,
			//		or the left parenthesis will be found
			// When left parenthesis is found it can be popped off the stack,
			//		assuming the stack is not empty
			while (topOperatorStackToken != "(")
			{
				outputQueue.push(topOperatorStackToken);
				operatorStack.pop();

				// If the stack is emptied break the loop
				if (operatorStack.empty())
					break;

				// Get new top token from operator stack
				// Cache it for future reference
				topOperatorStackToken = operatorStack.top();
			} // End while() loop; Checking for left bracket

			// Pop & discard the left bracket from the stack
			if (!operatorStack.empty())
				operatorStack.pop();

			// If the stack is empty & we never found the left bracket
			// There was a syntax error; missing bracket
			if (topOperatorStackToken != "(")
			{
				std::cout << "Failure Point: Mismatched parenthesis \n";
				return false;
			}	
		}

		// Otherwise, an invalid non-operator, non-integer token is present,
		//		that is also not the null terminator
		else if (currentToken != "\0")
		{
			std::cout << "Failure Point: Default condition \n";
			std::cout << currentToken;
			return false;
			
		}
	} // End for() loop; token iteration

	  // While there are still tokens in the operator stack
	  //		clear the operator stack and finalise the output queue
	while (!operatorStack.empty())
	{
		const std::string topOperatorStackToken = operatorStack.top();

		// If the operator on the top of the stack is a parenthesis,
		//		then there are mismatched parenthesis
		if (isParenthesis(topOperatorStackToken))
		{
			std::cout << "Failure Point: mismatched parenthesis";
			return false;
		}
			
		// Pop the operator onto the output queue
		outputQueue.push(topOperatorStackToken);
		operatorStack.pop();
	} // End while() loop; clear operator stack

	// Copy output queue contents to the return array
	while (!outputQueue.empty())
	{
		// Add front element of output queue to
		//		the back of the return array
		returnArray.push_back(outputQueue.front());

		// Pop front off of output queue
		outputQueue.pop();
	} // End while() loop; copying  output queue

	return true;
}
#pragma endregion

#pragma region RPN Calculation
class RPN
{
	public:
		bool calculatePostfix(const std::vector<std::string>& inputTokens, int& result);

	private:
		std::stack<int> valueStack;		// Holds values for Postfix calculations
};

/*	Function:	Calculate the result of an expression written in postfix notation
	Parameters:	1) Vector<string> ref inputTokens	- Reference to collection of postfix-ordered tokens
				2) int ref result					- Reference to the variable for storing results
													- Pass-by-reference used to directly modify the
														result value to be displayed withotu having to return
														a value & allows the method to return true/false for
														a successfuly calculation
	Returns: 1) bool	- Whether or not the calculation was successful
*/
bool RPN::calculatePostfix(const std::vector<std::string>& inputTokens, int& result)
{
	// Iterate over each given input token
	// Integer checked first as we know the integers are,
	//		ordered before operators thanks to Shunting Yard
	for (int i = 0; i < inputTokens.size(); i++)
	{
		const std::string currentToken = inputTokens[i];					// Cache the current token

		// Check if token is integer
		if (verifyInteger(currentToken))
		{
			// Convert string to integer,
			//		push to stack of values
			valueStack.push(std::stoi(currentToken));
		}

		// Check if token is an operator
		else if (isOperator(currentToken))
		{
			// All operators require two arguements
			// Check if there are enough values on the stack

			// If there is one value left on the stack,
			//		it is returned as the result
			if (valueStack.size() == 1)
			{
				result = valueStack.top();
				valueStack.pop();

				return true;
			}

			// If enough values
			if (valueStack.size() >= 2)
			{
				// Check which operator is currently selected
				// Pop required arguments off the value stack
				// Perform operation
				int arg1;
				int arg2;

				if (currentToken == "+")
				{				
					arg1 = valueStack.top();
					valueStack.pop();
					arg2 = valueStack.top();
					valueStack.pop();

					// Place the result back on top of the stack,
					//		to use in future operations
					int tmpResult = arg1 + arg2;
					valueStack.push(tmpResult);
				}

				else if (currentToken == "-")
				{
					arg1 = valueStack.top();
					valueStack.pop();
					arg2 = valueStack.top();
					valueStack.pop();

					// Place the result back on top of the stack,
					//		to use in future operations
					int tmpResult = arg1 - arg2;
					valueStack.push(tmpResult);
				}

				else if (currentToken == "*")
				{
					arg1 = valueStack.top();
					valueStack.pop();
					arg2 = valueStack.top();
					valueStack.pop();

					// Place the result back on top of the stack,
					//		to use in future operations
					int tmpResult = arg1 * arg2;
					valueStack.push(tmpResult);
				}
				
				if (currentToken == "/")
				{
					arg1 = valueStack.top();
					valueStack.pop();
					arg2 = valueStack.top();
					valueStack.pop();

					// Place the result back on top of the stack,
					//		to use in future operations
					// arg2 given first as the order for the division is
					//		reversed when placed on the stack
					int tmpResult = arg2 / arg1;
					valueStack.push(tmpResult);
				}
			}

			else
			{
				// Operation failed, return false
				std::cout << "Not enough arguements. Evaluation failed \n";
				return false;
			}
		}

		// Otherwise, something is wrong
		else
		{
			std::cout << "Invalid expression. Returning 0.";
			return false;
		}

	} // End for() loop - Iteration over input tokens

	// Set final result
	// Return true; successful evaluation
	result = valueStack.top();
	return true;
}
#pragma endregion

/*	Function:	- Evaluates a string-based mathematical expression
				- Tokenises string
				- Applies Shunting-Yard to create postfix expression
				- Calculates postfix expression
				- Provides results, and notifies if calculation (un)successful

	Parameters:	1) String ref expression	- Reference to the string defining the expression
				2) Int ref result			- Reference to the variable for storing results
											- Pass-by-reference used to directly modify the
												result value to be displayed withotu having to return
												a value & allows the method to return true/false for
												a successfuly calculation

	Returns:	bool - Whether or not the evaluation was successful
*/
bool evaluate(const std::string& expression, int &result)
{
	// Create new ShuntingYard instance to convert from
	//		infix to postfix notation
	ShuntingYard shunter;

	// Create new RPN instance to calculate results from
	//		the postfix expression
	RPN rpn;

	// Variable to store expression result
	int expressionResult;

	// Tokenise expression
	// istringstream used as it can easily separate the expression
	//		using whitespace
	// Disadvantage of requiring whitespace between all operators
	std::istringstream tokeniser(expression);
	std::vector<std::string> tokens;										// Vector to store expression tokens
	std::vector<std::string> shuntedOutput;									// Vector to store shunted postfix output

	// While we are reading the string
	while (tokeniser)
	{
		// Temporary string to store token
		std::string temp;

		// Store the currently retreived token in
		//		the temporary string
		tokeniser >> temp;

		// Add the currently retrieved token to
		//		the collection of tokens
		tokens.push_back(temp);
	}

	// Convert infix to RPN using Shunting Yard
	// Provide Shunting Yard with tokens to convert
	//		as well as a variable to store the output
	if (shunter.shuntInfixToRPN(tokens, shuntedOutput))
	{
		std::cout << "Expression successfully shunted \n";
	}

	else
	{
		std::cout << "\n Shunt failed \n";
		return false;
	}

	// Calculate result using RPN
	// Provide tokens to convert as well as a variable
	//		 to store the output
	if (rpn.calculatePostfix(shuntedOutput, result))
	{
		return true;
	}

	else
	{
		return false;
	}
}

int main()
{
	// Int to temporarily store results
	int result;

	// Expressions to test evaluate()
	std::string testExpressions[5] = {
		"1 + 3",
		"( 1 + 3 ) * 2",
		"( 4 / 2 ) + 6",
		"4 + ( 12 / ( 1 * 2 ) )",
		"( 1 + ( 12 * 2 )"
	};

	// Iterate through each expression
	for (int i = 0; i < 5; i++)
	{
		std::cout << "Evaluating: " << testExpressions[i] << "\n";

		// If the expression is successfully evaluated
		if (evaluate(testExpressions[i], result))
		{
			std::cout << "Evaluation Successful. Result: " << result << "\n";
		}

		else
		{
			std::cout << "Evaluation Failed.";
		}
	}

	return 0;
}

