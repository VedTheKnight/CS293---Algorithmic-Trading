#include "receiver.h"
#include <vector>
#include <string>
#include "trie.cpp"
#define ALPHABET_SIZE 26

using namespace std;

//______________________________________Utility Functions___________________________________________________________________________________________________

// removes /r and /n and extra spaces at the end of the line
void removeHiddenCharacters(std::string& str) {
    std::string result;
    for (char c : str) {
        if (c != '\r' && c != '\n') {
            result += c;
        }
    }
    str = result;
    int back = str.size()-1;
    while(str[back--] == ' '){
        str.pop_back();
    }
}

//simple function to convert string to integer both negative and positive
int stringToInt(const std::string& str) {
    int result = 0;
    for (char c : str) {
        if (c >= '0' && c <= '9') {
            // Convert the character to an integer and add it to the result
            result = result * 10 + (c - '0');
        } 
    }
    if(str[0]=='-')
        return -1*result;
    else    
        return result;
}

//last token doesnt include \n
vector<string> tokenize(string input){

    vector<string> tokens;

    size_t start = 0;
    size_t end = input.find(' ');

    while (end != string::npos) {
        // Extract the token from the input string
        string token = input.substr(start, end - start);

        // Add the token to the vector
        tokens.push_back(token);

        // Update the start and end positions for the next token
        start = end + 1;
        end = input.find(' ', start);
    }

    // Add the last token after the loop
    tokens.push_back(input.substr(start));

    return tokens;
}

//removes special characters from the input
string processOrder(string input){
    string result;
    for(char ch : input){
        if(std::isalnum(ch) || std::isspace(ch) || ch == '#'){
            result += ch;
        }
    }
    return result;
}

//normal power computation function computes a^b
int power(int a,int b){
    int res = 1;
    for(int i=0; i<b;i++){
        res*=a;
    }
    return res;
}

//function to convert a lower case string to upper case in place
void UpperCase(std::string& str) {
    for(int i=0;i<str.length();i++)
    {
        if (str[i] >= 'a' && str[i] <= 'z')   //checking for lowercase characters
        str[i] = str[i] - 32; 
    }
}

//______________________________________Useful Functions for Part 1________________________________________________________________________________________

//Trie Processing
bool orderoutput(string name,int price,char option,TrieNode* root)
{
    bool flag;
    
    TrieNode* query=search(root,name);
    if (query==NULL)//insert
    {
        insert_trie(root,name,vector<int>{price,0,0});//order is bestprice,bestbuy,bestsell
        flag= 1;return flag;
    }
    if (option=='b')
    {

        if (query->second[1]<price || query->second[1]==0)//price>best buy and is the new best order cancelling others
        {
            query->second[1]=price;
            if (query->second[1]==query->second[2])
            {
                query->second[1]=0;
                query->second[2]=0;//both trades cancel
                flag= 0;return flag;//best buy and best sell are same so output is (best price,0,0)
            }
            if(query->second[1]>query->second[0])//current trade is best buy and also better than best price 
            {
                query->second[0]=query->second[1];//its the new best price and no pending best buy
                query->second[1]=0;
                flag= 1;return flag;//best buy and best sell are same so output is (updated best price,0,bestsell/)
            }
            else//the current trade is best buy but not the better than best price
            {
                flag= 0;return flag;//output ->no change in your vector and new order just rejected
            }
        }
        else
        {
            flag= 0;return flag;
        }

    }
    if (option=='s')
    {
        if (query->second[2]>price ||query->second[2]==0)//price<best sell and is the new best order cancelling others
        {
            query->second[2]=price;
            if (query->second[1]==query->second[2])
            {
                query->second[1]=0;
                query->second[2]=0;//both trades cancel
                flag= 0;return flag;
            }
            if(query->second[2]<query->second[0])//current trade is best sell and also better than best price
            {
                query->second[0]=query->second[2];//its the new best price and no pending best buy
                query->second[2]=0;
                flag= 1;return flag;
            }
            else//the current trade is best buy but not the better than best price
            {
                flag= 0;return flag;
            }
        }
        else
        {
            flag= 0;return flag;//directly cancelled and not traded on
        }
    }

}

void process_problem1(TrieNode* root,string message)
{
    string order_name="",temp="";
    int price=0;
    int flagstr=0;

    char option;
    string pricestring="";
 
    vector<string>inputs=tokenize(message);
    removeHiddenCharacters(inputs[0]);
    order_name=inputs[0];
    UpperCase(inputs[0]);
    bool flag=orderoutput((inputs[0]),stringToInt(inputs[1]),inputs[2][0],root);
    option=inputs[2][0];
    price=stringToInt(inputs[1]);
    if (flag==1)
    {
        
        if (option=='b')
        cout <<order_name<<" "<< price<<" s"<<endl;
        else
        cout <<order_name<<" "<<price<<" b"<<endl;

    }
    else
    {
       cout<<"No Trade"<<endl;
    }

}

//______________________________________Useful Functions for Part 2________________________________________________________________________________________

//given a number n and k being the size of inputs upto that point, get binary representation of n
vector<int> getBinary(int n, int k){

    string binary="";

    while(n>0){
        binary = to_string(n%2) + binary;
        n = n/2;
    }


    vector<int> binary_vec(k,0);

    for(int i=0; i<binary.size(); i++){
        binary_vec[k-1-i] = binary[binary.size()-1-i] - 48;
    }
    
    return binary_vec;
}

//function to find token in tickers
int findTicker(vector<string> tickers, string token){
    for(int i=0;i<tickers.size(); i++){
        if(tickers[i] == token){
            return i;
        }
    }
    return -1;
}

// checks if a given combination of inputs is a zero sum combo
bool isZeroSum(vector<string> combination){

    vector<string> tickers;

    //first we filter out all the tickers
    for(auto line : combination){
        vector<string> tokens = tokenize(line);
        for(int i = 0; i<tokens.size()-2; i+=2){
            if(i%2 == 0 && findTicker(tickers,tokens[i])==-1){ //if string and it is not in tickers already we add it into tickers
                tickers.push_back(tokens[i]);
            }
        }
    }

    vector<int> corresponding_quantities(tickers.size(), 0);

    //now that we have the tickers we again go through the combinations and compute the corresponding quantities for the ticker
    for(auto line : combination){
        vector<string> tokens = tokenize(line);
        for(int i = 1; i<tokens.size()-2; i+=2){
            if(i%2 == 1){ //if string and it is not in tickers already we add it into tickers
                int it = findTicker(tickers, tokens[i-1]);
                if(tokens[tokens.size()-1][0] == 'b')
                    corresponding_quantities[it] += stringToInt(tokens[i]);
                else //in case it is a sell so we just invert it
                    corresponding_quantities[it] -= stringToInt(tokens[i]);
            }
        }
    }

    // now if all the corresponding quantities are zero then we are done
    for(auto i:corresponding_quantities){
        if(i!=0)
            return false;
    }
    return true;
}

//finds the best arbitrage given valid orders upto a point and the number of lines
vector<int> findBestArbitrageP2(vector<string> inputs, int k, int& total_profit){

    //code to find best arbitrage given k input lines

    vector<int> best_arbitrage(k,0);
    int max_profit = 0;

    int pow = 1;
    for(int i=1; i<=k;i++){
        pow*=2;
    } //gets 2^k which is the number of states we iterate over

    for(int arb=1; arb<pow; arb++){ //arbitrage 0 is useless so we go from 1 to 2^k-1
        //first we find the binary representation of the arbitrage "arb"
        vector<int> binary_arbitrage = getBinary(arb,k); //arb is an integer and k is the size of the vector we receive ultimately
        int profit = 0;

        // now we check if the given comibination of lines in put forms a positive sum arbitrage or not
        // if it does and sum > previous max sum we store it as the best arbitrage
        
        //get relevant lines
        vector<string> possible_arbitrage;
        for(int i=0; i<k; i++){
            if(binary_arbitrage[i]==1){
                possible_arbitrage.push_back(inputs[i]);
            }
        }

        //first we do a profit computation
        for(auto line: possible_arbitrage){
            vector<string> line_tokens = tokenize(line);
            if(line_tokens[line_tokens.size()-1][0] == 'b'){
                profit += stringToInt(line_tokens[line_tokens.size()-2]);
            }else{
                profit -= stringToInt(line_tokens[line_tokens.size()-2]);
            }
        }

        //now if this cost is greater than max_profit, and it is an arbitrage
        if(profit>max_profit && isZeroSum(possible_arbitrage)){
            max_profit = profit;    //update the max profit
            best_arbitrage = binary_arbitrage;
        }

    }

    total_profit+=max_profit;
    return best_arbitrage;
}

//checks equality between an order line and a stock bundle accounting for all permutations as well
bool checkEquality(string old_order, string name){

    bool flag = false;
   
    vector<string> tokens = tokenize(old_order);
    vector<string> name_tokens = tokenize(name);

    int counter = 0;

    for(int j = 0; j<tokens.size()-2; j++){
        for(int k = 0; k<name_tokens.size();k+=2){
            if(tokens[j] == name_tokens[k] && tokens[j+1] == name_tokens[k+1])
                counter++;
        }
    }

    if(counter == name_tokens.size()/2 && counter == (tokens.size()-2)/2){
        flag = true;
    }

    return flag;
}

// updates order book with new line after checking for all cancellations
void updateOrderBookP2(vector<string>& order_book, string input){
    
    vector<string> tokens = tokenize(input); //converts new orders into tokens 
    int price = stringToInt(tokens[tokens.size()-2]); //gets the price of order
    string name=""; // to store the name of the bundle
    char mode= tokens[tokens.size()-1][0];

    for(int i=0; i<tokens.size()-2;i++){
        name+=tokens[i];
        name+=" "; //adding space after every token
    }

    //gets atmost two matches with buy in first index and sell in second index
    int bestbuy = -1;
    vector<string> bbtokens;
    int bestsell = -1;
    vector<string> bstokens;

    for(int i=0; i<order_book.size(); i++){
        if(checkEquality(order_book[i],name)){ //if it matches we have possible cancellation
            vector<string> line_toks = tokenize(order_book[i]);
            if(line_toks[line_toks.size()-1][0] == 'b'){
                bestbuy = i;
                bbtokens = line_toks;
            }
            if(line_toks[line_toks.size()-1][0] == 's'){
                bestsell = i;
                bstokens = line_toks;
            }
        }
    }
    
    
    if(bestbuy == -1 && bestsell == -1){ //in this case we do not bother and simply add the line
        order_book.push_back(input);
    }

    else if(mode == 'b'){
        //first we compare with its own category
        if(bestbuy == -1){
            //this is the only buy so we check if there is a matching sell which exists otherwise we would not be here
            if(price == stringToInt(bstokens[bstokens.size()-2])){
                //we must cancel the sell and not push in the new order
                order_book.erase(order_book.begin()+bestsell);
            }
            else{ //if their prices do not match we can simply add the new order in
                order_book.push_back(input);
            }
        }
        if(bestbuy != -1){ //in which case we must first check with best buy prices and then best sell
            if(price <= stringToInt(bbtokens[bbtokens.size()-2])){ //if the new order has lower or equal price to the old best buy
                // we straight up dont care about the new order
            }
            else{
                // if the prices is more than the old best buy, the old best buy gets cancelled
                order_book.erase(order_book.begin()+bestbuy);

                // now we add a check if the new order cancels with any previous sell
                if(bestsell == -1){
                    // no cancellation simply add the new order
                    order_book.push_back(input);
                }
                else{
                    if(price == stringToInt(bstokens[bstokens.size()-2])){
                        //we must cancel the sell and not push in the new order
                        order_book.erase(order_book.begin()+bestsell);
                    }
                    else{ //if their prices do not match we can simply add the new order in
                        order_book.push_back(input);
                    }
                }
            }
        }
    }
    else if(mode == 's'){
        //first we compare with its own category
        if(bestsell == -1){
            //this is the only sell so we check if there is a matching buy which exists otherwise we would not be here
            if(price == stringToInt(bbtokens[bbtokens.size()-2])){
                //we must cancel the buy and not push in the new sell order
                order_book.erase(order_book.begin()+bestbuy);
            }
            else{ //if their prices do not match we can simply add the new order in
                order_book.push_back(input);
            }
        }
        if(bestsell != -1){ //in which case we must first check with best sell price and then best buy
            if(price >= stringToInt(bstokens[bstokens.size()-2])){ //if the new order has higher or equal price to the old best sell
                // we straight up dont care about the new order
            }
            else{
                // if the price is less than the old best sell, the old best sell gets cancelled
                order_book.erase(order_book.begin()+bestsell);

                // now we add a check if the new order cancels with any previous sell
                if(bestbuy == -1){
                    // no cancellation simply add the new order
                    order_book.push_back(input);
                }
                else{
                    if(price == stringToInt(bbtokens[bbtokens.size()-2])){
                        //we must cancel the buy and not push in the new order
                        order_book.erase(order_book.begin()+bestbuy);
                    }
                    else{ //if their prices do not match we can simply add the new order in
                        order_book.push_back(input);
                    }
                }
            }
        }
    }
    
}

//______________________________________Useful Functions for Part 3________________________________________________________________________________________

// recursive function to generate all possible quantities
void generateQuantities(vector<int>& inputArray, vector<int>& currentArray, int index, vector<vector<int>>& result) {
    if(index == inputArray.size()){
        result.push_back(currentArray);
        return;
    }

    for(int i = 1; i <= inputArray[index]; i++){
        currentArray[index] = i;
        generateQuantities(inputArray, currentArray, index + 1, result);
    }
}

//checks for matching buy and sell orders with 
bool checkBuySellPairs(string old_order, string name, int price, char mode){
    bool flag = false;
    
    vector<string> tokens = tokenize(old_order);
    vector<string> name_tokens = tokenize(name);

    //if the order mode is the same OR prices don't match we return false by default
    if(mode == tokens[tokens.size()-1][0] || price != stringToInt(tokens[tokens.size()-3])){ // i.e. this order isn't buy/sell compatible or prices arent exactly equal
        return flag;
    }

    //now check if it is a matching bundle
    int counter = 0;

    for(int j = 0; j<tokens.size()-3; j+=2){ // since we have one extra quantity term
        for(int k = 0; k<name_tokens.size();k+=2){
            if(tokens[j] == name_tokens[k] && tokens[j+1] == name_tokens[k+1])
                counter++;
        }
    }

    if(counter == name_tokens.size()/2 && counter == (tokens.size()-3)/2){ //add an extra condition to check for buy/sell compatibility
        flag = true;
    }

    return flag;
}

// updates order book with line after checking all the buy/sell orders
void updateOrderBookP3(vector<string>& order_book, string input){

    vector<string> tokens = tokenize(input); //converts new orders into tokens 
    int price = stringToInt(tokens[tokens.size()-3]); //gets the price of order
    int quantity = stringToInt(tokens[tokens.size()-2]); //gets the quantity of the order
    string name=""; // to store the name of the bundle
    char mode= tokens[tokens.size()-1][0]; //gets b/s

    for(int i=0; i<tokens.size()-3;i++){
        name+=tokens[i];
        name+=" "; //adding space after every token
    }

    int ctr = 0;
    int obs = order_book.size();
    for(int i=0; i<obs; i++){ // we check all the existing orders in the book
        if(order_book[i] == "X"){
            ctr++;
            continue;
        }
        if(checkBuySellPairs(order_book[i],name,price,mode)){ //if it matches we have possible cancellation
            vector<string> line_toks = tokenize(order_book[i]);
            
            if(stringToInt(line_toks[line_toks.size()-2]) > quantity){ //if the quantity of the order book line is more than the quantity of the new input line 
                int delta = stringToInt(line_toks[line_toks.size()-2]) - quantity;
                line_toks[line_toks.size()-2] = to_string(delta);
                string updated_entry="";
                // quantity is fully consumed so we dont need to bother about it. Simply adjust the order book entry to reflect the changes
                for(int j = 0; j<line_toks.size(); j++){
                    updated_entry+=line_toks[j];
                    if(j != line_toks.size()-1)
                        updated_entry+=" ";
                }
                order_book[i] = updated_entry;
                order_book.push_back("X");
                break;
            }
            else if(stringToInt(line_toks[line_toks.size()-2]) < quantity){
                order_book[i] = "X";
                // now adjust the quantity of the inputs and insert it into the order book
                int delta = stringToInt(tokens[tokens.size()-2]) - stringToInt(line_toks[line_toks.size()-2]);
                tokens[tokens.size()-2] = to_string(delta); 
                string updated_entry="";

                // quantity is fully consumed so we dont need to bother about it. Simply adjust the order book entry to reflect the changes
                for(int j = 0; j<tokens.size(); j++){
                    updated_entry+=tokens[j];
                    if(j != tokens.size()-1)
                        updated_entry+=" ";
                }

                input = updated_entry; //update the input
                if(order_book.size() == obs)
                    order_book.push_back(input);
                else
                    order_book[order_book.size()-1] = input;

            }
            else{ //both exactly cancel out and we just need to delete the old entry
                order_book[i] = "X";
                order_book.push_back("X");
                break;
            }
        }    
        else{
            ctr++;
        }
    }

    //now if there was no cancellation anywhere we simply want to add the stock to the orderbook
    //this takes care of the zero size case as well
    if(ctr == order_book.size()){
        order_book.push_back(input);
    }
}

// checks if a given combination of inputs could lead to a possible arbitrage and outputs null vector if no 
// and returns a vector of optimal quantities if yes. 
vector<int> checkArbitrage(vector<string> combination, int n, vector<int>& cache){

    vector<string> tickers;
    vector<int> quantities;

    //first we filter out all the tickers and all the quantities 
    for(auto line : combination){
        vector<string> tokens = tokenize(line);
        quantities.push_back(stringToInt(tokens[tokens.size()-2]));
        for(int i = 0; i<tokens.size()-3; i+=2){
            if(i%2 == 0 && findTicker(tickers,tokens[i])==-1){ //if string and it is not in tickers already we add it into tickers
                tickers.push_back(tokens[i]);
            }
        }
    }

    vector<vector<int>> quantity_combinations;

    // we need to fill in the code to get all the possible quantity combinations
    vector<int> currentArray(quantities.size(), 0);
    generateQuantities(quantities, currentArray, 0, quantity_combinations);

    int max_profit = 0;
    vector<int> best_combo = vector<int>(combination.size(),0);

    //now that we have the tickers we again go through the combinations and compute the corresponding quantities for the ticker
    for(auto quantity_combo : quantity_combinations){
        vector<int> corresponding_quantities(tickers.size(), 0);

        for(int j=0; j<combination.size(); j++){ //we go through all the lines so the correspondiing quantity is quantity_combo[j]
            string line = combination[j];
            vector<string> tokens = tokenize(line);
            for(int i = 1; i<tokens.size()-3; i+=2){
                if(i%2 == 1){ // update the quantities based on the bundle quantity and the quantity of the stock in the bundle itself
                    int it = findTicker(tickers, tokens[i-1]);
                    if(tokens[tokens.size()-1][0] == 'b')
                        corresponding_quantities[it] += quantity_combo[j]*stringToInt(tokens[i]);
                    else //in case it is a sell so we just invert it
                        corresponding_quantities[it] -= quantity_combo[j]*stringToInt(tokens[i]);
                }
            }
        }

        // now if all the corresponding quantities are zero then we are done
        int ctr = 0;
        for(auto i:corresponding_quantities){
            if(i==0)
                ctr++;
        }

        if(ctr == corresponding_quantities.size()){ //i.e there exists some combination that makes all the tickers have 0 quantity
            //must do cost computation
            int c = 0;
            int profit = 0;
            for(auto line: combination){
                vector<string> line_tokens = tokenize(line);
                if(line_tokens[line_tokens.size()-1][0] == 'b'){
                    profit += quantity_combo[c]*stringToInt(line_tokens[line_tokens.size()-3]); //price is at minus 3, quantity at minus 2
                }else{
                    profit -= quantity_combo[c]*stringToInt(line_tokens[line_tokens.size()-3]);
                }
                c++;
            }

            if(profit > max_profit){
                max_profit = profit;
                best_combo = quantity_combo;
            }
        }
        else{
            continue; // check other combinations
        }

    }

    if(max_profit>0){
        return best_combo;
    }

    //if we reach here means that there is not possibility for arbitrage
    cache[n] = -1;
    return vector<int>(combination.size(),0);
}

//finds the best arbitrage given valid orders upto a point and the number of lines
//returns the relevant orders and their corresponding quantities
vector<pair<int,int>> findBestArbitrageP3(vector<string> inputs, int n, int k, int& total_profit, vector<int>& cache){

    //code to find best arbitrage given k input lines

    vector<int> best_arbitrage(k,0);
    int max_profit = 0;
    vector<int> best_quantities(k,0);

    int pow = power(2,k);

    for(int arb=1; arb<pow; arb++){ //arbitrage 0 is useless so we go from 1 to 2^k-1
        //first we find the binary representation of the arbitrage "arb"
        vector<int> binary_arbitrage = getBinary(arb,k); //arb is an integer and k is the size of the vector we receive ultimately
        int profit = 0;

        // get relevant lines
        vector<string> possible_arbitrage;
        bool flag = 0;
        for(int i=0; i<k; i++){
            if(binary_arbitrage[i]==1){
                if(inputs[i] == "X"){
                    flag = 1;
                    break;
                }
                possible_arbitrage.push_back(inputs[i]);
            }
        }
        if(flag){
            continue; //we ignore that iteration if there is a X in the combination
        }
        if(cache[arb*power(2,n-k)] == -1){ //already checked and no arbitrage 
            //cout<<"REJECTED"<<endl;
            continue; 
        }
        //we first compute the possible linear combinations which satisfy are zero sum. 
        //calling this function gets us the optimal maximum quantities for the given combination corresponding to the
        //possible arbitrage vector
        
        vector<int> quantities = checkArbitrage(possible_arbitrage, arb*power(2,n-k), cache);

        int zcount = 0;
        for(int i = 0; i<quantities.size(); i++){
            if(quantities[i]==0)
                zcount++;
        }

        if(zcount == quantities.size()) //all zeros which means no arbitrage
            continue;
        
        //if we reach here it means we have a possible arbitrage on our hands
        //first we do a profit computation
        int ctr = 0;
        for(auto line: possible_arbitrage){
            vector<string> line_tokens = tokenize(line);
            if(line_tokens[line_tokens.size()-1][0] == 'b'){
                profit += quantities[ctr]*stringToInt(line_tokens[line_tokens.size()-3]); //price is at minus 3, quantity at minus 2
            }else{
                profit -= quantities[ctr]*stringToInt(line_tokens[line_tokens.size()-3]);
            }
            ctr++;
        }

        //now if this cost is greater than max_profit, and it is an arbitrage
        if(profit>max_profit && zcount != quantities.size()){
            max_profit = profit;    //update the max profit
            best_arbitrage = binary_arbitrage;
            int x = 0;
            for(int i=0; i<best_arbitrage.size(); i++){
                best_quantities[i] = (best_arbitrage[i] == 1)? quantities[x++]:0; //copies in the quantities vector
            }
        }
    }

    total_profit+=max_profit;
    vector<pair<int,int>> best;
    for(int i=0; i<best_arbitrage.size(); i++){
        best.push_back(make_pair(best_arbitrage[i], best_quantities[i]));
    }
    return best;
}


//______________________________________Logic for different parts______________________________________________________________________________________

void runPart1(){

    Receiver rcv;
    sleep(5);
    bool foundDollar = false;
    int iterator = 0;
    string message="";

    while(!foundDollar){
        std::string msg = rcv.readIML();
        message+=msg;
        if(msg.find('$')!= string::npos){
            rcv.terminate();
            foundDollar = true;
        }
    }

    //first we store each line in a vector of strings called inputs 

    vector<string> inputs;
    inputs.push_back("");
    int index = 0;  //index of the vector : inputs
    for(int i = 0; i<message.length(); i++){
        if(message[i]=='$'){    // edge case where we have the last line
            inputs.pop_back(); // pops the empty "" and breaks;
            break;
        }
        if(message[i]!='\r') //ignores random characters
            inputs[index].push_back(message[i]);
        if(message[i] == '#'){
            inputs.push_back("");
            index++;
        }
    }

    for(int i=0; i<inputs.size(); i++){
        removeHiddenCharacters(inputs[i]);
    }

    TrieNode* M = new TrieNode;

    for(int i=0;i<inputs.size();i++)
    {
        process_problem1(M,inputs[i]);
    }
}

void runPart2(){

    Receiver rcv;
    sleep(5);
    bool foundDollar = false;
    int iterator = 0;
    string message="";

    while(!foundDollar){
        std::string msg = rcv.readIML();
        message+=msg;
        if(msg.find('$')!= string::npos){
            rcv.terminate();
            foundDollar = true;
        }
    }

    //first we store each line in a vector of strings called inputs 

    vector<string> inputs;
    inputs.push_back("");
    int index = 0;  //index of the vector : inputs
    for(int i = 0; i<message.length(); i++){
        if(message[i]=='$'){    // edge case where we have the last line
            inputs.pop_back(); // pops the empty "" and breaks;
            break;
        }
        if(message[i]!='\r') //ignores random characters
            inputs[index].push_back(message[i]);
        if(message[i] == '#'){
            inputs.push_back("");
            index++;
        }
    }

    for(int i=0; i<inputs.size(); i++){
        removeHiddenCharacters(inputs[i]);
    }
    // Here we start the logic
    // If we have k lines in our input, we will do 2^k checks for arbitrage
    // In each check, we will go through all our tokens in the active strings and add it to a 
    // map: ticker->amnt assuming all as b orders. We check for cost and add it into a vector.
    
    vector<string> order_book;

    int total_profit = 0;

    for(int i=0; i<inputs.size(); i++){

        //NEED TO INSERT CODE FOR PROCESSING HERE FIRST -- WAIT FOR BRIAN LOGIC SIMILAR TO PHASE 1
        updateOrderBookP2(order_book, inputs[i]);

        //find the best arbitrage
        vector<int> best_arbitrage = findBestArbitrageP2(order_book, order_book.size(),total_profit); //given the number of lines

        bool flag = true;
        for(int j=0; j<best_arbitrage.size(); j++){
            if(best_arbitrage[j]==1){
                flag = false;
                break;
            }
        }

        if(flag){
            std::cout<<"No Trade\n";
            continue;
        }

        
        // if there exists an arbitrage
        // we display the arbitrage in reverse order
        for(int iterator = order_book.size()-1; iterator>=0; iterator--){
            if(best_arbitrage[iterator] == 1){ // this is a part of our arbitrage so we display it
                order_book[iterator][order_book[iterator].size()-2] = (order_book[iterator][order_book[iterator].size()-2]=='b')? 's':'b'; // assuming new line character is not there
                std::cout<<order_book[iterator].substr(0,order_book[iterator].length()-1)<<"\n";
            }
        }

        // we delete the lines from the order book in reverse order
        for(int j=order_book.size()-1; j>=0; j--){
            if(best_arbitrage[j]==1){
                order_book.erase(order_book.begin()+j);
            }
        }
    }

    std::cout<<total_profit;
}

void runPart3(){
    
    Receiver rcv;
    sleep(5);
    bool foundDollar = false;
    int iterator = 0;
    string message="";

    while(!foundDollar){
        string msg = rcv.readIML();
        message+=msg;
        if(msg.find('$')!= string::npos){
            rcv.terminate();
            foundDollar = true;
        }
    }

    // first we store each line in a vector of strings called inputs 

    vector<string> inputs;
    inputs.push_back("");
    int index = 0;  //index of the vector : inputs
    for(int i = 0; i<message.length(); i++){
        if(message[i]=='$'){    // edge case where we have the last line
            inputs.pop_back(); // pops the empty "" and breaks;
            break;
        }
        if(message[i]!='\r') //ignores random characters
            inputs[index].push_back(message[i]);
        if(message[i] == '#'){
            inputs.push_back("");
            index++;
        }
    }

    for(int i=0; i<inputs.size(); i++){
        removeHiddenCharacters(inputs[i]);
    }
    
    // here we store the logic for part3 which is similar to that for part 2 but the only difference being in the 
    // way we process the order book and treat arbitrages.

    vector<string> order_book;
    vector<int> cache = vector<int>(power(2,inputs.size()),0);
    int total_profit = 0;

    for(int i=0; i<inputs.size(); i++){

        //Create modified processing for this phase
        updateOrderBookP3(order_book, inputs[i]);

        //find the best arbitrage and update the quantities in the order book as well and update the total profit
        vector<pair<int,int>> best_arbitrage = findBestArbitrageP3(order_book,inputs.size(),order_book.size(),total_profit,cache); //given the number of lines
        //to check if valid arbitrage not present in which case output no trade and continue
        bool flag = true;
        for(int j=0; j<best_arbitrage.size(); j++){
            if(best_arbitrage[j].first==1){
                flag = false;
                break;
            }
        }

        if(flag){
            std::cout<<"No Trade\n";
            continue;
        }


        // if there exists an arbitrage
        // we display the arbitrage in reverse order
        // as we display we update the quantities in the order book and delete the ones where quantities become zero
        for(int iterator = order_book.size()-1; iterator>=0; iterator--){
            if(best_arbitrage[iterator].first == 1){ // this is a part of our arbitrage so we display it

                // we must modify this best arbitrage by subtracting the quantity returned
                vector<string> line_toks = tokenize(order_book[iterator]);
                int delta = stringToInt(line_toks[line_toks.size()-2]) - best_arbitrage[iterator].second; //subtract the quantities which will definitely be less than or equal to the orignal quantites
                line_toks[line_toks.size()-2] = to_string(delta); //this is the new updated
                
                string updated_entry="";
                string displayed_entry = "";
                // quantity is fully consumed so we dont need to bother about it. We simply adjust the order book entry to reflect the changes
                for(int j = 0; j<line_toks.size(); j++){
                    updated_entry+=line_toks[j];

                    if(j != line_toks.size()-2){
                        displayed_entry+=line_toks[j];
                    }
                    else{
                        displayed_entry+=to_string(best_arbitrage[iterator].second); //quantity traded
                    }

                    if(j != line_toks.size()-1){
                        updated_entry+=" ";
                        displayed_entry+=" ";
                    }
                }
                order_book[iterator] = updated_entry;
                //if delta is zero, i.e. the line is exhausted we must remove the line from the order book
                if(delta==0){
                    order_book[iterator] = "X";
                }
                
                displayed_entry[displayed_entry.size()-2] = (displayed_entry[displayed_entry.size()-2]=='b')? 's':'b'; // assuming new line character is not there
                std::cout<<displayed_entry.substr(0,displayed_entry.length()-1)<<"\n";
            }
        }
    }

    std::cout<<total_profit;
}

//______________________________________Main Function_______________________________________________________________________________________________________________

int main(int argc, char** argv) {
    if(argc != 2){
        cerr<<"Too many arguments!\n";
    }
    else{
        if(argv[1][0] == '1'){
            runPart1();
        }
        else if(argv[1][0] == '2'){
            runPart2();
        }
        else if(argv[1][0] == '3'){
            runPart3();
        }
        else
            cerr<<"Unexpected argument!\n";
    }

    return 0;
}

//_______________________________________________End___________________________________________________________________________________________________________