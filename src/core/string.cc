/**
 * @file string.cc
 * The implementation of string.hh
 */
#include "string.hh"
String::String(Maze m,Dirn stringDir,Dirn targetDir):maze(m),endPos(0,0,0),route(),stringDir(stringDir),targetDir(targetDir){
  Vector start=m.size().dotProduct(to_shift_vector(stringDir))*to_shift_vector(stringDir)+
      m.size().dotProduct(to_shift_vector(targetDir))*to_shift_vector(targetDir)+
      m.size().dotProduct(to_vector(perpendicular(stringDir,targetDir)))/2*to_vector(perpendicular(stringDir,targetDir));
  Vector pos=start;
  while(pos.dotProduct(to_vector(stringDir))!=(start+m.size()).dotProduct(to_vector(stringDir))){
    route.push_back(StringElement(pos,stringDir,true));
    pos+=to_vector(stringDir);
  }
  endPos=pos;
};

bool String::hasWon() const{
  Vector d=to_vector(targetDir);
  int t=maze.size().dotProduct(-to_shift_vector(opposite(targetDir)));
  if(endPos.dotProduct(d)<t)
    return false;
  for(std::list<StringElement>::const_iterator it=route.begin();it!=route.end();++it)
    if(it->pos.dotProduct(d)<t)
      return false;
  return true;
}

///A fixed size stack
template <class T>
class LimitedStack{
  private:
    SPA<T> start;///< The start of the stacks storage
    SPA<T> end;///< The end of the stacks storage
    SPA<T> top;///< The top of the stack
    SPA<T> bottom;///< The bottom of the stack

  public:
    /// Create a new stack
    /**
     * @param len the length of the new stack
     */
    LimitedStack(int len):start(len),end(start+len-1),top(start),bottom(start){};

    /// Check if the stack is empty
    /**
     * @return true if the stack is empty else false
     */
    bool empty(){
      return top==bottom;
    }

    /// Get the value at the top of the stack
    /**
     * @return the top element on the stack
     */
    inline const T& getTop(){
      return *top;
    }

    /// remove and return the value at the top of the stack
    /**
     * @return the top element on the stack
     */
    const T& popTop(){
      const T& tmp=*top;
      if(top==start)
        top=end;
      else
        --top;
      return tmp;
    }

    /// Add an element to the top of the stack
    /**
     * This may replace elements at the bottom of the stack if space runs out
     * @param el the element to add
     */
    void pushTop(const T& el){
      if(top==end){
        top=start;
        if(top==bottom)
          ++bottom;
      }else{
        ++top;
        if(top==bottom)
          if(bottom==end)
             bottom=top;
          else
             ++bottom;
      }
      *top=el;
    }

    ///Clear the stack
    /**
     * Doesn't actually delete any elements but they won't be accessed again
     */
    void clear(){
      top=bottom=start;
    }
};

///An element in the move history
class HistoryElement{
  public:
    int length; ///< The length of the string just after the move
    SPA<unsigned char> selected; ///< The selection state of the string just after the move
    /// was an element of the string selected
    /**
     * @param i the index of the element to check
     * @return true it was selected
     */
    bool isselected(int i){
      return (selected[i/CHAR_BIT]&(1<<(i%CHAR_BIT)))!=0;
    }
    Dirn d; ///< the direction the move was in
    std::list<Dirn> startcollapsed; ///< the route segments that where collapsed from the start of the string
    std::list<Dirn> endcollapsed; ///< the route segments that where collapsed from the end of the string
    
    /// Create a new history element
    /**
     * @param length The length of the string just after the move
     * @param selected The selection state of the string just after the move
     * @param d the direction the move was in
     * @param startcollapsed the route segments that where collapsed from the start of the string
     * @param endcollapsed the route segments that where collapsed from the end of the string
     */
    HistoryElement(int length,SPA<unsigned char> selected,Dirn d,std::list<Dirn> startcollapsed,std::list<Dirn> endcollapsed):
        length(length),selected(selected),d(d),startcollapsed(startcollapsed),endcollapsed(endcollapsed){};
    /// Create a new empty history element
    HistoryElement():length(0),selected(),d(),startcollapsed(),endcollapsed(){};
};

StringPlay::StringPlay(SP<String> s):s(s),score(0),undohistory(new LimitedStack<HistoryElement>(10+2*s->maze.size().dotProduct(Vector(1,1,1)))),inextendedmove(false){};

void StringPlay::SetString(SP<String> s){
  this->s=s;
  score=0;
  inextendedmove=false;
  delete undohistory;
  undohistory=new LimitedStack<HistoryElement>(10+2*s->maze.size().dotProduct(Vector(1,1,1)));
}

void StringPlay::externalEditHappened(){
  inextendedmove=false;
  undohistory->clear();
}
bool StringPlay::slide(bool moveEnd,bool out){
  if(moveEnd){
    std::list<StringElement>::reverse_iterator it;
    for(it=s->route.rbegin();it!=s->route.rend();++it)
      if(it->selected)
        break;
    if(out){
      if(it==s->route.rbegin())
        return false;
      --it;
      it->selected=true;
    }else{
      if(it==s->route.rend())
        return false;
      it->selected=false;
    }
  }else{
    std::list<StringElement>::iterator it;
    for(it=s->route.begin();it!=s->route.end();++it)
      if(it->selected)
        break;
    if(out){
      if(it==s->route.begin())
        return false;
      --it;
      it->selected=true;
    }else{
      if(it==s->route.end())
        return false;
      it->selected=false;
    }
  }
  inextendedmove=false;
  return true;
}

bool StringPlay::canMove(Dirn d){
  bool any=false;
  if((d==opposite(s->stringDir)&&s->route.front().selected)||(d==s->stringDir&&s->route.back().selected))
    return false;
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it){
    if(!it->selected)
      continue;
    any=true;
    if(d==UP && it->pos.Y>=s->maze.size().Y-1)
      return false;
    if(d==DOWN && it->pos.Y<=1)
      return false;
    if(d==LEFT && it->pos.X>=s->maze.size().X+5)
      return false;
    if(d==RIGHT && it->pos.X<=-5)
      return false;
    if(d==FORWARD && it->pos.Z>=s->maze.size().Z+5)
      return false;
    if(d==BACK && it->pos.Z<=-5)
      return false;
    if(it->d!=d && it->d!=opposite(d)){
      Vector wall=it->pos+to_shift_vector(it->d)+to_shift_vector(d);
      Dirn wallDirn=perpendicular(it->d,d);
      if(inCube(wall,Vector(0,0,0),s->maze.size())){
        if(((*s->maze[wall])&to_mask(wallDirn))!=0)
          return false;
      }
    }
  }
  return any;
}

void StringPlay::doMove(Dirn d,bool undo){
  int length=0;
  int movescore=0;
  bool lastselected=false;
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it){
    ++length;
    if(it->selected){
      if(!lastselected){
        if(it!=s->route.begin()){
          std::list<StringElement>::iterator nit=it;
          --nit;
          if(nit->d==opposite(d)){
            s->route.erase(nit);
            --length;
          }else{
            s->route.insert(it,StringElement(it->pos,d,false));
            ++length;
          }
        }else if(d==s->stringDir||d==opposite(s->stringDir))
          s->route.insert(it,StringElement(it->pos,d,false));
      }
      it->pos+=to_vector(d);
      if(it->d!=d && it->d!=opposite(d))
          movescore+=1;
    }else if(lastselected){
      if(it->d==d){
        it=s->route.erase(it);
        --length;
        if(it==s->route.end()){
          lastselected=false;
          break;
        }
      }else{
        s->route.insert(it,StringElement(it->pos+to_vector(d),opposite(d),false));
        ++length;
      }
    }
    lastselected=it->selected;
  }

  if(lastselected)
    if(d==s->stringDir||d==opposite(s->stringDir))
      s->route.insert(s->route.end(),StringElement(s->endPos+to_vector(d),opposite(d),false));
    else
      s->endPos+=to_vector(d);
  if(!undo){
    score+=movescore;
    SPA<unsigned char> selection((length+CHAR_BIT-1)/CHAR_BIT);
    int i=0;
    for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it,++i)
      if(it->selected)
        selection[i/CHAR_BIT]|=(1<<(i%CHAR_BIT));
    std::list<Dirn> startcollapsed;
    while(s->route.front().d!=s->stringDir){
      startcollapsed.push_back(s->route.front().d);
      s->route.pop_front();
    }
    std::list<Dirn> endcollapsed;
    while(s->route.back().d!=s->stringDir){
      endcollapsed.push_back(s->route.back().d);
      s->endPos=s->route.back().pos;
      s->route.pop_back();
    }
    undohistory->pushTop(HistoryElement(length,selection,d,startcollapsed,endcollapsed));
  }else
    score-=movescore;
}

bool StringPlay::undo(bool extendedmove){
  if(extendedmove && !inextendedmove)
    return false;
  inextendedmove=extendedmove;
  if(undohistory->empty())
    return false;
  HistoryElement el=undohistory->popTop();
  for(std::list<Dirn>::iterator it=el.startcollapsed.begin();it!=el.startcollapsed.end();++it){
    s->route.push_front(StringElement(s->route.front().pos-to_vector(*it),*it,false));
  }
  for(std::list<Dirn>::iterator it=el.endcollapsed.begin();it!=el.endcollapsed.end();++it){
    s->route.push_back(StringElement(s->endPos,*it,false));
    s->endPos+=to_vector(*it);
  }
  int i=0;
  for(std::list<StringElement>::iterator it=s->route.begin();it!=s->route.end();++it,++i)
    it->selected=el.isselected(i);
  doMove(opposite(el.d),true);
  return true;
}

bool StringPlay::tryMove(Dirn d,bool extendedmove){
  if(extendedmove && !inextendedmove)
    return false;
  inextendedmove=extendedmove;
  if(canMove(d)){
    doMove(d,false);
    return true;
  }else{
    return false;
  }
}

StringPlay::~StringPlay(){delete undohistory; };

void StringEdit::setStringSegment(StringPointer sp,StringPointer ep,int count,SPA<Dirn> newRoute){
  std::list<StringElement>::iterator it=sp.el;
  Vector pos=it->pos;
  bool endSel=true;
  if(ep!=s->end())
    endSel=ep.el->selected;
  for(SPA<Dirn> d=newRoute;d<newRoute+count;++it,++d){
    //run out of bits of string to move so add a new one
    if(it==ep.el){
      --it;
      bool sel=it->selected&&endSel;
      ++it;
      it=s->route.insert(it,StringElement(pos,*d,sel));
    }else{
      it->pos=pos;
      it->d=*d;
    }
    pos+=to_vector(*d);
  }
  {
    //connect up to the right distance across
    Dirn d=s->stringDir;
    int dist=to_vector(d).dotProduct((ep==s->end()?s->endPos:ep.el->pos)-pos);
    if(dist<0){
      dist=-dist;
      d=opposite(d);
    }
    for(int i=0;i<dist;++i,++it){
      if(it==ep.el){
        --it;
        bool sel=it->selected&&endSel;
        ++it;
        it=s->route.insert(it,StringElement(pos,d,sel));
      }else{
        it->pos=pos;
        it->d=d;
      }
      pos+=to_vector(d);
    }
  }
  //delete any spares
  while(it!=ep.el)
    it=s->route.erase(it);

  //slide the rest of the string across to line up
  for(it=ep.el;it!=s->route.end();++it){
    it->pos=pos;
    pos+=to_vector(it->d);
  }
  s->endPos=pos;
}
