#include <iostream>

using namespace std;

enum Color{RED=0,BLACK=1};  //红黑树结点颜色类型，0表示红色，1表示黑色

typedef struct Interval     //定义一个区间
{
    int low;                //区间的低端点
    int high;               //区间的高端点
}Interval;

typedef struct Node        //声明红黑树结点
{
    Color color;           //红黑树结点的颜色类型
    struct Node *parent;   //父节点
    struct Node *left;     //左孩子
    struct Node *right;    //右孩子
    Interval interval;     //区间
    int max;               //以该节点为根的子树中所有区间端点的最大值
}Node;

typedef struct RBTree      //定义一个红黑树
{
    Node *root;            //根节点
    Node *nil;             //叶节点
}RBTree;

//初始选项
void Display()
{

    cout<<"请选择操作(i：插入区间  s：查找区间)\n";
}

//求三个参数中的最大值
int GetMax(int high,int leftMax,int rightMax)
{
    int temp=(leftMax>rightMax)?leftMax:rightMax;
    return (high>temp)?high:temp;
}

//左旋
void LeftRotate(RBTree * rbTree,Node *x)
{
    if(x->right!=rbTree->nil)
    {
        Node *y=x->right;
        x->right=y->left;
        if(y->left!=rbTree->nil)
        {
            y->left->parent=x;
        }
        y->parent=x->parent;
        if(x->parent==rbTree->nil)    //空树，将y设为根
        {
            rbTree->root=y;
        }
        else if(x==x->parent->left)   //x为左子树，将y放在x父节点的左子树
        {
            x->parent->left=y;
        }
        else
        {
            x->parent->right=y;
        }
        y->left=x;
        x->parent=y;

        //以下为区间树与红黑树左旋调整的差异，即要调整结点max的大小,
        //且必须先计算x的max，在计算y的max
        x->max=GetMax(x->interval.high,x->left->max,x->right->max);
        y->max=GetMax(y->interval.high,y->left->max,y->right->max);
    }
}

//右旋
void RightRotate(RBTree * rbTree,Node *x)
{
    if(x->left!=rbTree->nil)
    {
        Node *y=x->left;
        x->left=y->right;
        if(y->right!=rbTree->nil)
        {
            y->right->parent=x;
        }

        y->parent=x->parent;
        if(x->parent==rbTree->nil)
        {
            rbTree->root=y;
        }
        else if(x==x->parent->left)
        {
            x->parent->left=y;
        }
        else
        {
            x->parent->right=y;
        }
        y->right=x;
        x->parent=y;

        x->max=GetMax(x->interval.high,x->left->max,x->right->max);
        y->max=GetMax(y->interval.high,y->left->max,y->right->max);
    }
}

//插入结点
void RBInsert(RBTree *rbTree,Interval interval)
{
    void RBInsertFixUp(RBTree *rbTree,Node *z);
    if(rbTree->root==NULL)
    {//当根为空时，单独处理，直接插入到根结点中
        rbTree->root=new Node;
        rbTree->nil=new Node;

        rbTree->root->left=rbTree->nil;
        rbTree->root->right=rbTree->nil;
        rbTree->root->parent=rbTree->nil;
        rbTree->root->interval.low=interval.low;      //设置区间低端点
        rbTree->root->interval.high=interval.high;    //设置区间高端点
        rbTree->root->max=interval.high;      //初始根的max设为自己的high

        rbTree->root->color=BLACK;            //根节点color设为黑

        rbTree->nil->parent=rbTree->root;
        rbTree->nil->left=rbTree->root;
        rbTree->nil->right=rbTree->root;
        rbTree->nil->interval.low=rbTree->nil->interval.high=-1;  //将nil的区间设为-1
        rbTree->nil->color=BLACK;     //nil结color也设为黑
        rbTree->root->max=0;          //nil节点的max设为0，便于其他节点max的维护

    }
    else
    {//如果树不为空，那么从根节点开始，从上往下查找插入点
        Node *y=rbTree->nil;     //y用于当前扫描结点x的父节点
        Node *x=rbTree->root;    //从根节点开始扫描
        while(x!=rbTree->nil)    //查找插入位置,以低端点为排序键值
        {
            if(interval.low==x->interval.low)
            {
                cout<<"键值重复，请输入不同的键值！！"<<endl;
                return;
            }
            y=x;
            x=interval.low<x->interval.low ? x->left : x->right;
        }
        Node *z=new Node;       //new一个Node结点空间
        z->color=RED;           //新插入的color设为红色
        z->interval.low=interval.low;
        z->interval.high=interval.high;
        z->left=z->right=rbTree->nil;
        z->max=GetMax(interval.high,z->left->max,z->right->max);
        z->parent=y;
        if(interval.low<y->interval.low)
            y->left=z;
        else
            y->right=z;

        RBInsertFixUp(rbTree,z);   //插入后对树进行调整
    }
}

//插入后调整树，以满足红黑树的5条性质
void RBInsertFixUp(RBTree *rbTree,Node *z)
{
    Node *y;      //用于记录z的叔叔结点
    while(z->parent->color==RED)   //因为插入的结点是红色的，所以只可能违背性质4,即假如父结点也是红色的，要做调整
    {
        if(z->parent->parent->left==z->parent)  //如果要插入的结点z是其父结点的左子树
        {
            y=z->parent->parent->right;         // y设置为z的叔父结点
            if(y->color==RED)                   //case 1: y的颜色为红色
            {
                z->parent->parent->color=RED;
                y->color=BLACK;
                z->parent->color=BLACK;
                z=z->parent->parent;
            }
            else
            {
                if(z==z->parent->right)    //case 2: y的颜色为黑色，并且z是z的父母的右结点，则z左旋转
                {
                    z=z->parent;
                    LeftRotate(rbTree,z);
                }
                z->parent->parent->color=RED;     //case 3: 如果y的颜色为黑色，并且z是z的父母的左结点
                z->parent->color=BLACK;
                RightRotate(rbTree,z->parent->parent);
            }
        }
        else    //与前一种情况对称，要插入的结点z是其父结点的右子树,注释略去
        {
            y=z->parent->parent->left;
            if(y->color==RED)
            {
                z->parent->parent->color=RED;
                y->color=BLACK;
                z->parent->color=BLACK;
                z=z->parent->parent;
            }
            else
            {
                if(z->parent->left==z)
                {
                    z=z->parent;
                    RightRotate(rbTree,z);
                }
                z->parent->parent->color=RED;
                z->parent->color=BLACK;
                LeftRotate(rbTree,z->parent->parent);
            }
        }
    }
    rbTree->root->color=BLACK;   //最后如果上升为rbTree的根的话，把根的颜色设为黑色

}

//查找与给定区间重叠的区间
Node* IntervalSearch(RBTree * rbTree,Interval interval)
{
    Node *x=rbTree->root;    //从根开始查找
    while(x!=rbTree->nil&&!(interval.low<=x->interval.high&&interval.high>=x->interval.low))
    {//若x不等于nil节点且x与interval不重叠，则进行判断
        if(x->left!=rbTree->nil&&x->left->max>=interval.low)
            x=x->left;       //到x的左子树中继续查找
        else
            x=x->right;      //左子树必查不到，到右子树查
    }
    return x;    //x=nil或者x与interval重叠
}


int main() {
    RBTree tree;
    tree.root = tree.nil = NULL;
    char choose;
    Interval interval;
    while (true) {
        Display();
        cin >> choose;
        switch (choose) {
            case 'i': {//选择1则插入结点
                cout << "请输入区间的左右两个端点(如：1 10）:\n";
                cin >> interval.low >> interval.high;
                RBInsert(&tree, interval);
                cout << endl;
                break;
            }
            case 's': {//显示查找重叠区间的结果
                cout << "请输入待查找的区间(如：1 10）：\n";
                cin >> interval.low >> interval.high;
                Node *x = IntervalSearch(&tree, interval);
                if (x != tree.nil) {
                    cout << "\n重叠区间为：" << "[" << x->interval.low << "," << x->interval.high << "]\n" << endl;
                } else
                    cout << "\n无重叠区间！" << endl << endl;
            }
        }
    }
}