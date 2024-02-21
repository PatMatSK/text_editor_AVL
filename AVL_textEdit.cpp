#ifndef __PROGTEST__
#include <cassert>
#include <iomanip>
#include <cstdint>
#include <iostream>
#include <memory>
#include <limits>
#include <optional>
#include <algorithm>
#include <bitset>
#include <list>
#include <array>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <queue>
#include <random>
#include <type_traits>

#endif
using namespace std;

int max(int a, int b) {
    return a > b ? a : b;
}

/// --------------------------------------------------------------------------------------------------------------------
/// ----------------------------------------------NODE------------------------------------------------------------------
class TNode {
public:
                    TNode           (char a, TNode * d) :   c(a), prevCCnt(0), prevLnCnt(0), depth(1),
                                                            daddy(d), left(nullptr), right(nullptr){}
                    ~TNode          () { delete left; delete right; }
    void            propagateNewline(); // tell fathers that he became newline
    void            propagateNewChar(); // tell fathers that he became newline
    void            propagateDelNewline();
    void            propagateDelChar();
    TNode *         balance         ();
    int             getStatus       () const;
    void            updateDepth     () ;
    TNode *         rotateRight     ();
    TNode *         rotateLeft      ();
    char            c;
    unsigned int    prevCCnt;
    unsigned int    prevLnCnt;
    int             depth;
    TNode *         daddy;
    TNode *         left;
    TNode *         right;
};


TNode * TNode::balance( ) {
    TNode * tmp = this;
    TNode * prevDaddy = nullptr;
    int prevDiff = 0;
    int diff;
    TNode * res = nullptr;
    while ( tmp ) {

        tmp->updateDepth();
        diff = tmp->getStatus();
        if ( diff == 2 ) {
            prevDiff = tmp->right->getStatus();
            if ( prevDiff == -1 )
                tmp->right = tmp->right->rotateRight();
            prevDaddy = tmp->daddy;

            res = tmp->rotateLeft();

            if ( prevDaddy && prevDaddy->left == tmp )
                prevDaddy->left = res;
            else if ( prevDaddy && prevDaddy->right == tmp )
                prevDaddy->right = res;
            tmp = res;
        }


        if ( diff == -2 ) {
            prevDiff = tmp->left->getStatus();
            if ( prevDiff == 1 )
                tmp->left = tmp->left->rotateLeft();
            prevDaddy = tmp->daddy;
            res = tmp->rotateRight();
            if ( prevDaddy && prevDaddy->left == tmp )
                prevDaddy->left = res;
            else if ( prevDaddy && prevDaddy->right == tmp )
                prevDaddy->right = res;
            tmp = res;
        }

        if (!tmp->daddy)
            break;
        tmp = tmp->daddy;
    }
    return tmp;
}


int TNode::getStatus() const {
    int lt, rt;
    if ( left )
        lt = left->depth;
    else
        lt = 0;
    if ( right )
        rt = right->depth;
    else
        rt = 0;
    return rt - lt;
}

void TNode::updateDepth()  {
    int lt, rt;
    if ( left )
        lt = left->depth;
    else
        lt = 0;
    if ( right )
        rt = right->depth;
    else
        rt = 0;
    depth = max( rt, lt) + 1;
}


TNode * TNode::rotateRight() {
    TNode * newroot = left;

    prevCCnt = prevCCnt - newroot->prevCCnt - 1;

    prevLnCnt = prevLnCnt - newroot->prevLnCnt;
    if ( left->c == '\n' )
        prevLnCnt--;

    left = left->right;
    if (newroot->right)
        newroot->right->daddy = this;
    newroot->right = this;
    newroot->daddy = daddy;
    daddy = newroot;

    updateDepth();
    newroot->updateDepth();
    return newroot;
}

TNode * TNode::rotateLeft() {
    TNode * newroot = right;

    newroot->prevCCnt += prevCCnt+1;

    newroot->prevLnCnt += prevLnCnt;
    if ( c == '\n' )
        newroot->prevLnCnt++;

    right = right->left;
    if (newroot->left)
        newroot->left->daddy = this;
    newroot->left = this;
    newroot->daddy = daddy;
    daddy = newroot;

    updateDepth();
    newroot->updateDepth();
    return newroot;
}


void TNode::propagateNewline() {
    TNode * prev = this;
    TNode * tmp = daddy;
    while ( tmp ) {
        if ( tmp->left == prev )
            tmp->prevLnCnt++;

        prev = tmp;
        tmp = tmp->daddy;

    }
}

void TNode::propagateNewChar() {
    TNode * prev = this;
    TNode * tmp = daddy;
    while ( tmp ) {
        if ( tmp->left == prev )
            tmp->prevCCnt++;

        prev = tmp;
        tmp = tmp->daddy;

    }
}

void TNode::propagateDelNewline() {
    TNode * prev = this;
    TNode * tmp = daddy;
    while ( tmp ) {
        if ( tmp->left == prev )
            tmp->prevLnCnt--;
        prev = tmp;
        tmp = tmp->daddy;
    }
}

void TNode::propagateDelChar() {
    TNode * prev = this;
    TNode * tmp = daddy;
    while ( tmp ) {
        if ( tmp->left == prev )
            tmp->prevCCnt--;
        prev = tmp;
        tmp = tmp->daddy;
    }
}



/// --------------------------------------------------------------------------------------------------------------------
/// ------------------------------------------GENERAL-------------------------------------------------------------------
size_t findMostLefty(TNode * tmp, size_t index) {      // in this line
    TNode * prev = tmp;
    TNode * tmp2 = tmp->left;
    size_t i = index;

    while ( tmp2 && tmp2->prevLnCnt == tmp->prevLnCnt ) {
        i = i - prev->prevCCnt + tmp2->prevCCnt;
        prev = tmp2;
        tmp2 = tmp2->left;
    }

    if ( !tmp2 || !tmp2->right )
        return i;

    size_t j = i - prev->prevCCnt + tmp2->prevCCnt;
    size_t lc = tmp2->prevLnCnt;
    prev = tmp2;
    tmp2 = tmp2->right;
    while ( tmp2 && lc != tmp->prevLnCnt ) {
        j += tmp2->prevCCnt + 1;
        lc += tmp2->prevLnCnt;
        if ( prev->c == '\n')
            lc++;
        prev = tmp2;
        tmp2 = tmp2->right;
    }

    if ( !tmp2 && lc != tmp->prevLnCnt )
        return i;

    return findMostLefty(prev, j);
}


/// --------------------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------------------
/// ----------------------------------------------EDITOR----------------------------------------------------------------
struct TextEditorBackend {
public:
                TextEditorBackend       (const std::string& text);
                ~TextEditorBackend      () { delete root; }
    size_t      size                    () const;
    size_t      lines                   () const;

    char        at                      (size_t i) const;
    void        edit                    (size_t i, char c);
    void        insert                  (size_t i, char c);
    void        erase                   (size_t i);

    size_t      line_start              (size_t r) const;
    size_t      line_length             (size_t r) const;
    size_t      char_to_line            (size_t i) const;
private:
    void        loadRoot                (char c);
    void        loadNode                (TNode * tmp, char c);
    void        loadLastNode            (TNode * tmp, char c);
    void        eliminateHim            (TNode * tmp);
    void        eliminateMiddle         (TNode * tmp);

    size_t          linesCnt;
    size_t          count;
    TNode *         root;
    bool            lastEnter;
};

TextEditorBackend::TextEditorBackend(const std::string &text): linesCnt(0), count(0), root( nullptr), lastEnter(false) {
    size_t index = 0;
    for ( auto c : text )
        insert(index++, c);
}

size_t TextEditorBackend::size() const {
    return count;
}

size_t TextEditorBackend::lines() const {
    if (!root)
        return 1;

    return lastEnter ? linesCnt+1 : linesCnt;
}


char TextEditorBackend::at(size_t i) const {
    if ( i < 0 || i > count )
        throw std::out_of_range("");

    TNode * tmp = root;
    size_t index = 0;
    while (tmp) {
        index += tmp->prevCCnt;
        if ( index == i )
            return tmp->c;
        if ( i < index ) {
            index -= tmp->prevCCnt;
            tmp = tmp->left;
        }
        else {
            ++index;
            tmp = tmp->right;
        }
    }

    throw std::out_of_range("jebe ci?");
}


void TextEditorBackend::edit(size_t i, char c) {
    if ( i < 0 || i >= count )
        throw std::out_of_range("jebe ci?");

    TNode * tmp = root;
    size_t index = 0;
    while (tmp) {
        index += tmp->prevCCnt;
        if ( index == i )
            break;
        if ( i < index ) {
            index -= tmp->prevCCnt;
            tmp = tmp->left;
        }
        else {
            ++index;
            tmp = tmp->right;
        }
    }
    if ( ! tmp )
        throw std::out_of_range("jebe ci?");

    if ( c == tmp->c )
        return;

    if ( c == '\n' ) {
        if ( i + 1 != count )
            linesCnt++;
    }

    else if ( c != '\n' && tmp->c == '\n' ) {
        if ( i + 1 != count )
            linesCnt--;
        tmp->propagateDelNewline();
    }
    if ( index+1 == count ) {
        if ( c == '\n' )
            lastEnter = true;
        else
            lastEnter = false;
    }
    if ( c == '\n' && tmp->c != '\n' )
        tmp->propagateNewline();

    tmp->c = c;
}


size_t TextEditorBackend::line_start(size_t r) const {
    if ( r > linesCnt )
        throw std::out_of_range("");

    if ( r == 0 )
        return 0;
    TNode * tmp = root;
    size_t lineIndex = 0;
    size_t index = 0;
    while (tmp) {
        lineIndex += tmp->prevLnCnt;
        index += tmp->prevCCnt;
        if ( lineIndex == r )
            return findMostLefty(tmp, index);
        else if ( r < lineIndex ) {
            lineIndex -= tmp->prevLnCnt;
            index -= tmp->prevCCnt;
            tmp = tmp->left;
        }
        else {
            ++index;
            if ( tmp->c == '\n')
                ++lineIndex;

            if ( lineIndex == r )
                return index;

            tmp = tmp->right;
        }
    }
    throw std::out_of_range("wrong index");
}

size_t TextEditorBackend::line_length(size_t r) const {
    if ( r == linesCnt )
        return 0;

    if ( linesCnt == 1 && r == 0 )
        return count;

    if ( r >= linesCnt || r < 0 )
        throw std::out_of_range("jebe ci?");
    if ( r + 1 == linesCnt )
        return count - line_start(r);

    return line_start(r+1) - line_start(r);
}


size_t TextEditorBackend::char_to_line(size_t i) const {
    if ( i < 0 || i >= count )
        throw std::out_of_range("jebe ci?");

    TNode * tmp = root;
    size_t lineIndex = 0;
    size_t index = 0;
    while (tmp) {
        lineIndex += tmp->prevLnCnt;
        index += tmp->prevCCnt;
        if ( index == i ) {
            return lineIndex;
        }
        if ( i < index ) {
            lineIndex -= tmp->prevLnCnt;
            index -= tmp->prevCCnt;
            tmp = tmp->left;
        }
        else {
            ++index;
            if ( tmp->c == '\n'){
                ++lineIndex;
            }
            tmp = tmp->right;
        }
    }

    throw std::out_of_range("jebe ci?");
}

void TextEditorBackend::loadRoot(char c) {
    root = new TNode(c, nullptr);
    count = linesCnt = 1;
    if ( c == '\n' )
        lastEnter = true;
    else
        lastEnter = false;
}


void TextEditorBackend::loadNode(TNode * tmp, char c) {
    if ( tmp->left ) {
        tmp = tmp->left;
        while (tmp->right)
            tmp = tmp->right;
        tmp->right = new TNode(c, tmp);
        tmp = tmp->right;
    } else {
        tmp->left = new TNode(c, tmp);
        tmp = tmp->left;
    }
    if ( c == '\n' ) {
        linesCnt++;
        tmp->propagateNewline();
    }
    tmp->propagateNewChar();
    root = tmp->balance();
}

void TextEditorBackend::loadLastNode(TNode * tmp, char c) {
    tmp->right = new TNode(c, tmp);
    if (lastEnter)
        linesCnt++;
    if ( c == '\n' ){
        tmp->right->propagateNewline();
        lastEnter = true;
    }
    else
        lastEnter = false;

    tmp->right->propagateNewChar();
    root = tmp->right->balance();
}


void TextEditorBackend::insert(size_t i, char c) {
    if ( i > count || i < 0 )
        throw std::out_of_range("insert");

    if ( !root && i == 0 )
        return loadRoot(c);

    TNode * tmp = root;
    size_t index = 0;
    while ( tmp ) {
        index += tmp->prevCCnt;
        if ( i == index ) {
            loadNode(tmp, c);
            break;
        }
        else if ( i < index ) {
            index -= tmp->prevCCnt;
            tmp = tmp->left;
        }
        else {
            index++;
            if ( !tmp->right && index == i ) {
                loadLastNode(tmp, c);
                break;
            }
            tmp = tmp->right;
        }
    }
    count++;
}


TNode * helping(TNode * tmp, TNode * sub ){

    tmp->propagateDelChar();
    if ( tmp->c == '\n' )
        tmp->propagateDelNewline();

    if ( !tmp->daddy ) {                    // holding root - must have only one son
        sub->daddy = nullptr;
        tmp->left = tmp->right = nullptr;
        delete tmp;
        return sub->balance();
    }

    TNode * toBalance = tmp->daddy;

    if ( tmp->daddy->left == tmp )
        tmp->daddy->left = sub;
    else
        tmp->daddy->right = sub;

    if (sub) {
        toBalance = sub;
        sub->daddy = tmp->daddy;
    }
    tmp->left = tmp->right = nullptr;

    delete tmp;

    if ( toBalance->left )
        toBalance = toBalance->left;
    if ( toBalance->right )
        toBalance = toBalance->right;

    return toBalance->balance();
}

void TextEditorBackend::eliminateMiddle( TNode * tmp ) {       // has 2 sons

    TNode * sub = tmp->right;
    int counter = 0;
    while (sub->left) {
        counter++;
        sub = sub->left;
    }
    char c = tmp->c;
    tmp->c = sub->c;
    sub->c = c;
    // ------------------------------------
    if ( tmp->c == '\n' && sub->c != '\n' ){
        TNode * goUp = sub->daddy;
        while ( counter ){
            goUp->prevLnCnt--;
            goUp = goUp->daddy;
            counter--;
        }
    }
    else if ( sub->c == '\n' && tmp->c != '\n' ){
        tmp->propagateDelNewline();
        sub->c = 'q';
    }
    // ------------------------------------
    eliminateHim(sub);
}

void TextEditorBackend::eliminateHim(TNode * tmp) {
    if ( !tmp->right && !tmp->left )
        root = helping(tmp, nullptr);
    else if ( !tmp->right )
        root = helping(tmp, tmp->left);
    else if ( !tmp->left )
        root = helping(tmp, tmp->right);
    else
        eliminateMiddle(tmp);
}


void TextEditorBackend::erase(size_t i) {
    if ( i < 0 || i >= count )
        throw std::out_of_range("erase");

    if (count == 1) {
        delete root;
        root = nullptr;
        count = linesCnt = 0;
        lastEnter = false;
        return;
    }
    TNode * tmp = root;
    size_t index = 0;
    while (tmp) {
        index += tmp->prevCCnt;
        if ( index == i )
            break;
        if ( i < index ) {
            index -= tmp->prevCCnt;
            tmp = tmp->left;
        }
        else {
            ++index;
            tmp = tmp->right;
        }
    }

    if ( tmp->c == '\n' )
        linesCnt--;

    if ( index == count-1 ) {
        char preLast = at(index-1);
        if ( tmp->c == '\n' && preLast != '\n' ) {  //mam posledny prvok '\n' a pred nim neni '\n'
            linesCnt++;  // canter of condition few rows above
            lastEnter = false;
        }
        if ( tmp->c != '\n' && preLast == '\n' )
            lastEnter = true;
    }
    eliminateHim(tmp);
    count--;

}





/// --------------------------------------------------------------------------------------------------------------------
/// --------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__

////////////////// Dark magic, ignore ////////////////////////

template < typename T >
auto quote(const T& t) { return t; }

std::string quote(const std::string& s) {
    std::string ret = "\"";
    for (char c : s) if (c != '\n') ret += c; else ret += "\\n";
    return ret + "\"";
}

#define STR_(a) #a
#define STR(a) STR_(a)

#define CHECK_(a, b, a_str, b_str) do { \
    auto _a = (a); \
    decltype(a) _b = (b); \
    if (_a != _b) { \
      std::cout << "Line " << __LINE__ << ": Assertion " \
        << a_str << " == " << b_str << " failed!" \
        << " (lhs: " << quote(_a) << ")" << std::endl; \
      fail++; \
    } else ok++; \
  } while (0)

#define CHECK(a, b) CHECK_(a, b, #a, #b)

#define CHECK_ALL(expr, ...) do { \
    std::array _arr = { __VA_ARGS__ }; \
    for (size_t _i = 0; _i < _arr.size(); _i++) \
      CHECK_((expr)(_i), _arr[_i], STR(expr) "(" << _i << ")", _arr[_i]); \
  } while (0)

#define CHECK_EX(expr, ex) do { \
    try { \
      (expr); \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but no exception was raised." << std::endl; \
    } catch (const ex&) { ok++; \
    } catch (...) { \
      fail++; \
      std::cout << "Line " << __LINE__ << ": Expected " STR(expr) \
        " to throw " #ex " but got different exception." << std::endl; \
    } \
  } while (0)

////////////////// End of dark magic ////////////////////////


std::string text(const TextEditorBackend& t) {
    std::string ret;
    for (size_t i = 0; i < t.size(); i++) ret.push_back(t.at(i));
    return ret;
}

void test1(int& ok, int& fail) {
    TextEditorBackend s("123\n456\n789");
    CHECK(s.size(), 11);
    CHECK(text(s), "123\n456\n789");
    CHECK(s.lines(), 3);
    CHECK_ALL(s.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2);
    CHECK_ALL(s.line_start, 0, 4, 8);
    CHECK_ALL(s.line_length, 4, 4, 3);
}

void test2(int& ok, int& fail) {
    TextEditorBackend t("123\n456\n789\n");
    CHECK(t.size(), 12);
    CHECK(text(t), "123\n456\n789\n");
    CHECK(t.lines(), 4);
    CHECK_ALL(t.char_to_line, 0,0,0,0, 1,1,1,1, 2,2,2,2);
    CHECK_ALL(t.line_start, 0, 4, 8, 12);
    CHECK_ALL(t.line_length, 4, 4, 4, 0);
}

void test3(int& ok, int& fail) {
    TextEditorBackend t("asdfasdfasdf");

    CHECK(t.size(), 12);
    CHECK(text(t), "asdfasdfasdf");
    CHECK(t.lines(), 1);
    CHECK_ALL(t.char_to_line, 0,0,0,0, 0,0,0,0, 0,0,0,0);
    CHECK(t.line_start(0), 0);
    CHECK(t.line_length(0), 12);
    t.insert(0, '\n');
    CHECK(t.size(), 13);

    CHECK(text(t), "\nasdfasdfasdf");
    CHECK(t.lines(), 2);
    CHECK_ALL(t.line_start, 0, 1);

    t.insert(4, '\n');
    CHECK(t.size(), 14);
    CHECK(text(t), "\nasd\nfasdfasdf");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.insert(t.size(), '\n');
    CHECK(t.size(), 15);
    CHECK(text(t), "\nasd\nfasdfasdf\n");
    CHECK(t.lines(), 4);
    CHECK_ALL(t.line_start, 0, 1, 5, 15);

    t.edit(t.size() - 1, 'H');
    CHECK(t.size(), 15);
    CHECK(text(t), "\nasd\nfasdfasdfH");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.erase(8);
    CHECK(t.size(), 14);
    CHECK(text(t), "\nasd\nfasfasdfH");
    CHECK(t.lines(), 3);
    CHECK_ALL(t.line_start, 0, 1, 5);

    t.erase(4);
    CHECK(t.size(), 13);
    CHECK(text(t), "\nasdfasfasdfH");
    CHECK(t.lines(), 2);
    CHECK_ALL(t.line_start, 0, 1);

}

void test_ex(int& ok, int& fail) {
    TextEditorBackend t("123\n456\n789\n");
    CHECK_EX(t.at(12), std::out_of_range);

    CHECK_EX(t.insert(13, 'a'), std::out_of_range);
    CHECK_EX(t.edit(12, 'x'), std::out_of_range);
    CHECK_EX(t.erase(12), std::out_of_range);

    CHECK_EX(t.line_start(4), std::out_of_range);
    CHECK_EX(t.line_start(40), std::out_of_range);
    CHECK_EX(t.line_length(4), std::out_of_range);
    CHECK_EX(t.line_length(6), std::out_of_range);
    CHECK_EX(t.char_to_line(12), std::out_of_range);
    CHECK_EX(t.char_to_line(25), std::out_of_range);
}

int main() {

    int ok = 0, fail = 0;
    if (!fail) test1(ok, fail);
    if (!fail) test2(ok, fail);
    if (!fail) test3(ok, fail);

    if (!fail) test_ex(ok, fail);
    if (!fail) std::cout << "Passed all " << ok << " tests!" << std::endl;
    else std::cout << "Failed " << fail << " of " << (ok + fail) << " tests." << std::endl;

    TextEditorBackend t1("asdfasdfasdf\n");

    for ( int i =0; i < 11; i++ ) {
        t1.insert(i, '\n');
        assert( t1.line_start(1) == i+1 );
        t1.erase(i);
    }

    TextEditorBackend t2("\n\n\n\n\n\n\n\n\n\n");
    for ( int i = 0; i < 10; i++ ) {
        assert( t2.line_start(i) == i );
        assert( t2.line_length(i) == 1);
    }
    assert( t2.line_start(10) == 10);
    assert( t2.line_length(10) == 0);


    TextEditorBackend a("asdfasdfasdf");
    a.insert( 4, 'p');
    a.edit(3, '\n');
    assert( a.line_start(0) == 0);
    assert( a.line_start(1) == 4);


    TextEditorBackend b("asdfgasdfgasdfgasdfgasdfgasdfga");
    b.edit(3, '\n');
    assert( b.line_start(0) == 0);
    assert( b.line_start(1) == 4);
    b.edit(2, '\n');
    b.erase(6);
    b.erase(4);
    b.erase(4);
    assert( b.line_start(0) == 0);
    assert( b.line_start(1) == 3);
    assert( b.line_start(2) == 4);
    b.erase(2);
    assert( b.line_start(0) == 0);
    assert( b.line_start(1) == 3);
    b.insert(2, '\n');
    assert( b.line_start(0) == 0);
    assert( b.line_start(1) == 3);
    assert( b.line_start(2) == 4);
    b.edit(20, '\n');
    assert( b.line_start(3) == 21);
    b.erase(17);
    assert( b.line_start(3) == 20);
    b.insert(b.size(), '\n');
    b.insert(b.size(), '1');
    b.insert(b.size(), '2');
    b.insert(b.size(), '2');
    b.insert(b.size(), '2');
    b.insert(b.size(), '2');
    b.insert(b.size(), '2');
    b.erase(b.size()-1);

    TextEditorBackend c("\n2");

    assert( c.char_to_line(0) == 0);
    c.erase(0);

    assert( c.size() == 1);
    assert( c.lines() == 1);
    assert( c.char_to_line(0) == 0);
    c.insert(1, '\n');

    assert( c.lines() == 2 );
    c.erase(1);
    assert( c.lines() == 1 );
    c.edit(0, '\n');

    assert( c.lines() == 2 );
    assert( c.line_start(0) == 0);
    assert( c.line_start(1) == 1);

    TextEditorBackend w("");
    w.insert(0, 'z');
    w.insert(0, 't');
    w.insert(0, 'r');
    w.insert(0, 'e');
    w.insert(0, 'w');
    w.insert(0, 'q');
    w.edit(0, '\n');
    w.insert(2, 'p');
    assert( w.line_start(0) == 0);
    assert( w.line_start(1) == 1);
    w.edit(0, 'w');
    w.edit(1, '\n');
    assert( w.line_start(0) == 0);
    assert( w.line_start(1) == 2);

    TextEditorBackend q("");
    int top = 4;
    for ( int i = 0; i < top; i++ )
        q.insert(i, 'q');

    for ( int i = 0; i < top; i++ ) {
        q.edit(i, '\n');
        assert( q.line_start(1) == i+1 );
        q.edit(i, 'q');
    }

    for ( int i = 0; i < top; i++ ) {
        q.insert(i, '\n');
        assert( q.line_start(1) == i+1 );
        q.erase(i);
    }

    TextEditorBackend t("");
    t.insert(0, 'a');
    t.insert(1, 'b');
    t.insert(2, 'c');
    assert( t.line_length(0) == 3);
    assert( t.line_length(1) == 0);
    t.edit(0, '\n');
    assert( t.line_length(0) == 1);
    assert( t.line_length(1) == 2);
    assert( t.line_length(2) == 0);
    t.edit(2, '\n');
    assert( t.line_length(0) == 1);
    assert( t.line_length(1) == 2);
    assert( t.line_length(2) == 0);
    t.erase(0);
    assert( t.line_length(0) == 2);
    assert( t.line_length(1) == 0);


    TextEditorBackend tt("\n");
    assert( tt.lines() == 2 );
    assert( tt.line_start(0) == 0);
    assert( tt.line_start(1) == 1);
    assert( tt.line_length(0) == 1);
    assert( tt.line_length(1) == 0);


    TextEditorBackend biggy("");
    for ( int i = 0; i < 63; i++)
        biggy.insert(0, 'q');
    biggy.edit(20, '\n');
    assert( biggy.line_start(1) == 21);

    TextEditorBackend te("");

    int max = 1000;
    te.insert( 0 , 'd');
    for ( int i = 1; i < max; i++)
        te.insert( rand() % te.size() , 'd');

    for ( int i = 0; i < te.size(); i++)
        assert( 'd' == te.at(i));

    for ( int i = 0; i < max; i++){
        size_t j = rand() % (te.size()-2);
        te.edit(j, '\n');
        assert( te.line_start(1) == j+1 );
        for ( int k = 0; k <= j ; k++ )
            assert( te.char_to_line(k) == 0 );

        for ( int k = j+1; k < te.size() ; k++ )
            assert( te.char_to_line(k) == 1 );

        size_t l = rand() % (te.size()-2);
        te.edit(l, '\n');

        if ( l < j ) {
            assert( te.line_start(0) == 0 );
            assert( te.line_start(1) == l+1 );
            assert( te.line_start(2) == j+1 );
            for ( int k = 0; k <= l ; k++ )
                assert( te.char_to_line(k) == 0 );
            for ( int k = l+1; k <= j ; k++ )
                assert( te.char_to_line(k) == 1 );
            for ( int k = j+1; k < te.size() ; k++ )
                assert( te.char_to_line(k) == 2 );
        }
        else if ( l > j ){
            assert( te.line_start(0) == 0 );
            assert( te.line_start(1) == j+1 );
            assert( te.line_start(2) == l+1 );
            for ( int k = 0; k <= j ; k++ )
                assert( te.char_to_line(k) == 0 );
            for ( int k = j+1; k <= l ; k++ )
                assert( te.char_to_line(k) == 1 );
            for ( int k = l+1; k < te.size() ; k++ )
                assert( te.char_to_line(k) == 2 );
        }
        te.edit(l, 'q');
        te.edit(j, 'q');
    }


    for ( int i = 0; i < max; i++){
        size_t j = rand() % (te.size()-2);
        te.insert(j, '\n');
        assert( te.line_start(1) == j+1 );

        for ( int k = 0; k <= j ; k++ ){
            assert( te.char_to_line(k) == 0 );
        }

        for ( int k = j+1; k < te.size() ; k++ )
            assert( te.char_to_line(k) == 1 );



        size_t l = rand() % (te.size()-2);
        te.insert(l, '\n');

        if ( l < j ) {
            assert( te.line_start(0) == 0 );
            assert( te.line_start(1) == l+1 );
            assert( te.line_start(2) == j+2 );
            for ( int k = 0; k <= l ; k++ )
                assert( te.char_to_line(k) == 0 );
            for ( int k = l+1; k <= j+1 ; k++ )
                assert( te.char_to_line(k) == 1 );
            for ( int k = j+2; k < te.size() ; k++ )
                assert( te.char_to_line(k) == 2 );
        }
        else if ( l > j ){
            assert( te.line_start(0) == 0 );
            assert( te.line_start(1) == j+1 );
            assert( te.line_start(2) == l+1 );
            for ( int k = 0; k <= j ; k++ )
                assert( te.char_to_line(k) == 0 );
            for ( int k = j+1; k <= l ; k++ )
                assert( te.char_to_line(k) == 1 );
            for ( int k = l+1; k < te.size() ; k++ )
                assert( te.char_to_line(k) == 2 );

        }
        te.erase(l);
        te.erase(j);
    }

    for ( int i = 0; i < max; i++){
        size_t j = rand() % (te.size());
        if ( te.size() == 1 )
            j = 0;
        te.erase( j );
        for ( int k = 0; k < te.size(); k++)
            assert( te.at(k) );
    }
}

#endif


