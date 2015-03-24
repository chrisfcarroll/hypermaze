#include "vector.hh"
#include "dirns.hh"
#include "maze.hh"
#include <list>

#ifdef IOSTREAM
#include <istream>
#endif
#ifndef STRING_HH_INC
#define STRING_HH_INC

// forward declarations so we can refer to them mainly for friend declarations
class StringPlay;
class StringEdit;

struct StringElement{
  Vector pos;
  Dirn d;
  bool selected;
  StringElement(Vector pos,Dirn d,bool selected):pos(pos),d(d),selected(selected){};
};

class StringPointer{
  private:
    std::list<StringElement>::iterator el;
  public:

    const StringElement& operator *()const{
      return *el;
    }
    const StringElement* operator ->() const{
      return el.operator->();
    }

    StringPointer& operator++(){
      ++el;
      return *this;
    }
    StringPointer& operator--(){
      --el;
      return *this;
    }

    bool operator!=(const StringPointer& other) const{
      return el!=other.el;
    }
    bool operator==(const StringPointer& other) const{
      return el==other.el;
    }

    StringPointer& operator=(StringPointer other){
      el=other.el;
      return *this;
    }

    StringPointer(std::list<StringElement>::iterator el):el(el){};

    friend class StringPlay;
    friend class StringEdit;
};

class ConstStringPointer{
  private:
    std::list<StringElement>::const_iterator el;
  public:

    const StringElement& operator *()const{
      return *el;
    }
    const StringElement* operator ->()const{
      return el.operator->();
    }

    ConstStringPointer& operator++(){
      ++el;
      return *this;
    }
    ConstStringPointer& operator--(){
      --el;
      return *this;
    }

    bool operator!=(const ConstStringPointer& other) const{
      return el!=other.el;
    }
    bool operator==(const ConstStringPointer& other) const{
      return el==other.el;
    }

    ConstStringPointer& operator=(const ConstStringPointer& other){
      el=other.el;
      return *this;
    }

    ConstStringPointer(std::list<StringElement>::const_iterator el):el(el){};
};

#ifdef IOSTREAM
inline std::ostream& operator<<(std::ostream& o,const StringElement& e){
  return o<<"<StringElement "<<e.pos<<" "<<e.d<<" "<<e.selected<<">";
}
inline std::ostream& operator<<(std::ostream& o,const StringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
inline std::ostream& operator<<(std::ostream& o,const ConstStringPointer& e){
  return o<<"<Pointer to "<<*e<<">";
}
#endif
class String{
    std::list<StringElement> route;
    Vector endPos;
  public:
    Maze maze;
    const Dirn stringDir;
    const Dirn targetDir;

    String(Maze m);

    inline const Vector& getStart() const{
      return route.front().pos;
    }
    inline const Vector& getEnd() const{
      return endPos;
    }

    inline const std::list<StringElement>& getRoute() const{
      return route;
    }

    inline StringPointer begin(){
      return StringPointer(route.begin());
    }
    inline StringPointer end(){
      return StringPointer(route.end());
    }
    inline ConstStringPointer begin() const{
      return ConstStringPointer(route.begin());
    }
    inline ConstStringPointer end() const{
      return ConstStringPointer(route.end());
    }

    inline int length() const{
      return route.size();
    }

    bool hasWon() const;

    inline String& operator=(const String& o){
      endPos=o.endPos;
      route=o.route;
      maze=o.maze;
      return *this;
    }

    friend class StringPlay;
    friend class StringEdit;
    #ifdef IOSTREAM
    friend std::ostream& operator <<(std::ostream& o,String s);
    friend std::ostream& operator <<(std::ostream& o,StringPlay s);
    #endif
};
// forward declare
template <class T>
class LimitedStack;
class HistoryElement;

class StringPlay{
  String& s;
  int score;

  LimitedStack<HistoryElement>* undohistory;
  bool inextendedmove;

  public:
    StringPlay(String& s);

    inline String& getString(){
      return s;
    }

    inline int getScore(){
      return score;
    }

    inline void startExtendedMove(){
      inextendedmove=true;
    }

    void externalEditHappened();

    bool slide(bool moveEnd,bool out);

    void setSelected(StringPointer p,bool selected){
      inextendedmove=false;
      p.el->selected=selected;
    }

    bool canMove(Dirn d);

  private:
    void doMove(Dirn d,bool undo);

  public:
    bool undo(bool extendedmove=false);

    bool tryMove(Dirn d,bool extendedmove=false);

    #ifdef IOSTREAM
    friend std::ostream& operator <<(std::ostream& o,StringPlay s);
    #endif
    StringPlay(const StringPlay& o);

    StringPlay& operator=(const StringPlay& o);
    ~StringPlay();
};

class StringEdit{
  String& s;

  public:
    StringEdit(String& s):s(s){};

    inline String& getString(){
      return s;
    }

    inline void setSelected(StringPointer p,bool selected){
      p.el->selected=selected;
    }

    void setStringSegment(StringPointer sp,StringPointer ep,int count,SPA<Dirn> newRoute);
};

#ifdef IOSTREAM
inline std::ostream& operator<<(std::ostream& o,String s){
  o<<"<String ";
  for(std::list<StringElement>::iterator it=s.route.begin();it!=s.route.end();++it)
    o<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return o<<s.endPos<<">";
}

inline std::ostream& operator<<(std::ostream& o,StringPlay s){
  o<<"<StringPlay ";
  for(std::list<StringElement>::iterator it=s.s.route.begin();it!=s.s.route.end();++it)
    o<<it->pos<<"-"<<(it->selected?"":"*")<<it->d<<(it->selected?"":"*")<<"-";
  return o<<s.s.endPos<<">";
}
#endif

#endif

