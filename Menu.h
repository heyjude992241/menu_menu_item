class MenuItem{
  private:
    uint8_t _order;
    uint8_t _coor_x, _coor_y;
    uint8_t _page;
    bool _isHasNextPage;
    unsigned char _function;

  public:
    MenuItem(){} //default constructor
    MenuItem(uint8_t x, uint8_t y, uint8_t order, uint8_t page, bool nextPage, unsigned char fnct){
      _coor_x = x;
      _coor_y = y;
      _page = page;
      _order = order;
      _isHasNextPage = nextPage;
      _function = fnct;
    }

    unsigned char getFunction(){
      return _function;
    }

    uint8_t getOrder(){
      return _order;
    }

    uint8_t getX(){
      return _coor_x;
    }

    uint8_t getY(){
      return _coor_y;
    }

    uint8_t getPage(){
      return _page;
    }

    bool isHasNextPage(){
      return _isHasNextPage;
    }
};


class Menu{
  private:
    bool _isHasNext, _isHasPrev;
    MenuItem* _item;
    uint8_t _order;
    uint8_t _item_count;

  public:
    Menu(){}
    Menu(MenuItem* item, uint8_t item_count, uint8_t order, bool isNext, bool isPrev){
      _item = item;
      _order = order;
      _item_count = item_count;
      _isHasNext = isNext;
      _isHasPrev = isPrev;
    }

    uint8_t getItemCount(){
      return _item_count;
    }
    
    bool getIsHasNext(){
      return _isHasNext;
    }

    bool getIsHasPrev(){
      return _isHasPrev;
    }

    MenuItem* getMenuItem(){
      return _item;
    }

    uint8_t getOrder(){
      return _order;
    }
};
