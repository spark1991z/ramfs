#include <iostream>
#include <map>
#include <vector>

using namespace std;

namespace fs {
   static const string DIR_DELIM = "/";
   enum fs_t {
      FS_DIR,
      FS_FILE,
      FS_EXEC
   };

   class FSNode {
      private:
         const fs_t _type;
         string _name;
         FSNode *_parent;
         map<string,FSNode*> _nodes;

         void update() {
            if(_parent!=NULL)
               _parent->update(this);
         }
         void update(FSNode*_node) {
            if(_node!=NULL && _node!=this && _node!=NULL && _nodes.find(_node->_name)!=_nodes.end()){
               _nodes[_node->_name] = _node;
               cout << __func__ << ": Updating " << _node->_name << " into " << _name << endl;
            }
         }
      public:
         static const bool touch(FSNode*_to, FSNode*_node) {
            if(_to!=NULL && _node!=NULL && _to!=_node) {
               if(_to->_nodes.find(_node->_name)!=_to->_nodes.end())
                  return false;
               cout << __func__ << ": Touching " << _node->_name << " into " << _to->_name << endl;
               _to->_nodes.insert(pair<string,FSNode*>(_node->_name,_node));
               _to->update();
               return true;
            }
            return false;
         }
         static const bool touch(FSNode*_node, const fs_t&_type, const string&_name) {
            if(_node!=NULL && _node->_type==FS_DIR && _name!="." && _name!="..") {
               for(uint8_t i=0;i<_name.length();i++)
                  if(_name[i] == '/') return false;
               return touch(_node,new FSNode(_type,_name,_node));
            }
            return false;
         }
         static FSNode* root(FSNode*_node) {
            if(_node->_parent!=NULL)
               return root(_node->_parent);
            return _node;
         }
         static FSNode* node(FSNode*_node, const string&_name) {
            return _node!=NULL && _node->_nodes.find(_name)!=_node->_nodes.end() ? _node->_nodes[_name] : NULL;
         }
         static FSNode* find(FSNode*_node, const string&_path) {
            FSNode* _n = NULL;
            if(_node!=NULL && &_path!=NULL) {
               _n = _node;
               string _name;
               for(uint8_t i=0;i<_path.length()+1;i++){
                  if(_path[i]=='/' || i==_path.length()) {
                     cout << "Checking: " << _name << endl;
                     if(_name.length()<1) {
                        if(_n == _node) {
                           _n = root(_n);
                        }
                        continue;
                     }
                     if(_name == ".") {
                        if(_n == NULL)
                           _n = _node;
                        _name = "";
                        continue;
                     }
                     if(_name==".." && _node->_parent!=NULL){
                        _n = _node->_parent;
                        _name = "";
                        continue;
                     }
                     _n = node(_n,_name);
                     if(_n == NULL) break;
                     _name = "";
                     continue;
                  }
                  _name += _path[i];
               }
            }
            return _n;
         }
         static const string path(const FSNode*_node) {
            string p = _node!=NULL && _node->_parent!=NULL ? path(_node->_parent) : "";
            if(_node!=NULL){
                  p+= _node->_name;
                  if(_node->_type == FS_DIR)
                     p+= DIR_DELIM;
            }
            return p;
         }

         FSNode(const fs_t&_type, const string&_name, FSNode*_parent)
         : _type(_type), _name(_name), _parent(_parent){}

   };

   class RAMFS {

   };
}
using namespace fs;

static FSNode *_root;

 void mkRootFS() {
   if(_root!=NULL) return;
   _root = new FSNode(FS_DIR,"",NULL);
   FSNode::touch(_root,FS_DIR,"bin");
   FSNode::touch(_root,FS_DIR,"dev");
   FSNode::touch(_root,FS_DIR,"usr");
}

int main()
{
   mkRootFS();
   FSNode* dev = FSNode::find(_root,"dev");
   if(dev!=NULL) cout << FSNode::path(dev) << endl;

   return 0;
}
