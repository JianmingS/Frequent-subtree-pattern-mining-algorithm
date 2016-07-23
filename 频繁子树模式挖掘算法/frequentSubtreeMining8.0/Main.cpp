#define HOME

#ifdef HOME
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <queue>
#include <stdexcept>
using namespace std;


// 节点
struct Node
{
	size_t id;	// 唯一标识，也即：在数组中的位置
	int father;	// 父节点唯一标识，也即：在数组中父节点的下标
	vector<string> properties;	// 数组下标即id，保存特征
	vector<size_t> sonNode;	// 孩子节点，也即：在数组中孩子节点的下标

	size_t orderID;	// 该节点在句子树中的顺序ID

	// 处理数据树中的 “T模式树” 节点
	string uuidByMatchProperty; // 当该节点作为“模式树”中的节点时，属性集合
	int availableFather;	// 如果当前点available，更新后的父节点
};


// 节点（模式树T）
struct NodeForT
{
	size_t id;	// 唯一标识
	int father;	// 父节点唯一标识，也即：在数组中父节点的下标

	string uuidByMatchProperty;	// 属性集合

	unsigned long long propertyFlag = 0;	// 标记节点中被选取的属性（默认从第一个属性开始取，直到所有属性取完）
};

// 数据树中子树T
struct SubTree
{
	size_t itemNo;	// 子树所属的数据树编号
	size_t root;	// 根节点位置
	size_t rml;	// 最右叶节点位置
	unordered_map<size_t, int> toFatherID;	// 数据树中对应的模式树T的父节点
};

// 树的信息
struct TreeMessage
{
	int nodeCnt = 0;	// 节点数目
};


// 需要输入的数据
vector<vector<Node>> Data;	// 输入的数据集
unsigned propertyNum;	// 属性的个数
vector<size_t> toMatchProperties;	// 求多维频繁子树时，需要相匹配的属性集
unsigned int minsupp;	// 最小支持度阈值


// 统计数目
int intputDataNum = 0;	// 输入数据数目
int frequentSubtreeNum = 0;	// 频繁子树个数


// 属性信息
struct PropertyMeg
{
	unsigned long long propertyFlag = 0;
	unsigned cnt = 0;
};

unordered_map<string, PropertyMeg> frequentProperty;	//【频繁属性集合】相关信息

TreeMessage TtreeMeg;	// 模式树节点以及层数信息

vector<SubTree> itemToSubTr;	// 每个数据树中记录子树T位置

// 模式树的根节点和最右叶节点标识
string Troot, Trml;

vector<size_t> visitDataPos;	// 需要访问的数据树的下标

// 统计数目
inline int &calDataNum(int &num)
{
	++num;
	return num;
}


/********************************************* 输入数据 ************************************************/
// 设置属性的个数
inline void setPropertyNum()
{
	propertyNum = 6;
}


// 设置求多维频繁子树时，需要相匹配的属性集
inline void setToMatchProperties()
{
	//vector<size_t> matchProperty = { 0, 1, 2, 5 };
	//vector<size_t> matchProperty = { 1, 2, 5 };
	vector<size_t> matchProperty = { 1, 2 };
	//vector<size_t> matchProperty = { 1 };
	for (size_t i = 0; i != matchProperty.size(); ++i)
	{
		toMatchProperties.push_back(matchProperty[i]);
	}
	sort(toMatchProperties.begin(), toMatchProperties.end());
}

// 设置最小支持度阈值
inline void setMinsupp()
{
	minsupp = 2;
}


// 输入数据
inline void inputData()
{
	int iid;
	vector<Node> Item;
	//std::ios::sync_with_stdio(false);
	while (cin >> iid)
	{
		if (!iid && !Item.empty())
		{
			calDataNum(intputDataNum);
			//cout << "intputDataNum :" << calDataNum(intputDataNum) << endl;
			Data.push_back(Item);
			Item.clear();
		}
		Node nd;
		nd.id = iid;
		nd.properties.push_back(std::to_string(iid));
		string strTmp;
		for (size_t i = 1; i < propertyNum; ++i)
		{
			cin >> strTmp;
			nd.properties.push_back(strTmp);
		}
		nd.father = std::stoi(nd.properties[propertyNum - 2]);
		Item.push_back(nd);
	}
	if (!Item.empty())
	{
		cout << "intputDataNum :" << calDataNum(intputDataNum) << endl;
		Data.push_back(Item);
		Item.clear();
	}
}

// 设置数据库中所有数据树的节点顺序（顺便保存每个节点的孩子节点）
inline void setDataTreeOrderID()
{
	for (size_t i = 0; i != Data.size(); ++i)
	{
		vector<Node> &Item = Data[i];

		vector<bool> visited(Item.size());
		fill(visited.begin(), visited.end(), false);

		size_t orderID = 0;

		queue<size_t> queNode;

		for (size_t j = 0; j != Item.size(); ++j)
		{
			Node &nd = Item[j];
			if (nd.father < 0)
			{
				nd.orderID = orderID;
				++orderID;
				visited[j] = true;
				queNode.push(j);
			}
		}

		/*
			代码中可以如此设定顺序的原因：
				在数据库中的多维树，其节点数组下标即为节点ID，两者可以合二为一处理。
		*/
		while (!queNode.empty())
		{
			size_t fatherID = queNode.front();
			queNode.pop();

			for (size_t j = 0; j != Item.size(); ++j)
			{
				if (!visited[j])
				{
					Node &nd = Item[j];
					if (nd.father == fatherID)
					{
						Item[nd.father].sonNode.push_back(j);
						nd.orderID = orderID;
						++orderID;
						visited[j] = true;
						queNode.push(nd.id);
					}
				}
			}
		}
	}
}
/********************************************* 输入数据 ************************************************/


/********************************************* 输出数据 ************************************************/
// 输出频繁属性集合
inline void outputFrequentProperty()
{
	int propertyStrNum = 0;
	for (unordered_map<string, PropertyMeg>::const_iterator cit = frequentProperty.begin();
		cit != frequentProperty.end(); ++cit)
	{
		++propertyStrNum;
		cout << propertyStrNum << ": " << (*cit).first << endl;
	}
}

// 输出频繁子树（即最后的结果）
inline void outputFrequentSubtree(const vector<NodeForT> &Ttree)
{
	cout << "frequentSubtreeNum : " << calDataNum(frequentSubtreeNum) << " : " << endl;
	for (vector<NodeForT>::size_type i = 0; i != Ttree.size(); ++i)
	{
		cout << Ttree[i].uuidByMatchProperty << endl;
		//cout << Ttree[i].id << " " << Ttree[i].uuidByMatchProperty
		//	<< " " << Ttree[i].father << endl;
	}
	cout << endl;
}

/********************************************* 输出数据 ************************************************/



/********************************************* 获取所有属性集合 ************************************************/
// 获取单独属性出现次数
inline void getSinglePropertySetWithCnt(unordered_map<string, PropertyMeg> &siglePropertySet)
{
	for (size_t i = 0; i < Data.size(); ++i)
	{
		for (size_t j = 0; j < Data[i].size(); ++j)
		{
			Node &nd = Data[i][j];
			for (size_t k = 0; k != toMatchProperties.size(); ++k)
			{
				const string &propertyStr = nd.properties[toMatchProperties[k]];
				if (siglePropertySet.find(propertyStr) == siglePropertySet.end())
				{
					siglePropertySet[propertyStr].cnt = 1;
					siglePropertySet[propertyStr].propertyFlag = static_cast<unsigned long long>(1 << toMatchProperties[k]);
				}
				else
				{
					try
					{
						if (siglePropertySet[propertyStr].propertyFlag != static_cast<unsigned long long>(1 << toMatchProperties[k]))
						{
							throw runtime_error("属性集没有按要求顺序输入（或者属性集不相互独立）");
						}
					}
					catch (runtime_error err)
					{
						cerr << "Error: " << __FILE__
							<< " : in function " << __func__
							<< " at line " << __LINE__ << endl
							<< "     Comiled on " << __DATE__
							<< " at " << __TIME__ << endl
							<< err.what() << endl;
					}
					++siglePropertySet[propertyStr].cnt;
				}
			}
		}
	}
}


/*
方法一：枚举属性集合+剪枝(利用频繁单属性)
适用于：属性个数较少，直接枚举属性的复杂度小于遍历数据库的复杂度
*/

// 获取所有属性集合
inline void getAllPropertySet(size_t num, size_t pos, const size_t &K, const Node &nd,
	const unordered_map<string, PropertyMeg> &siglePropertySet,
	string strProperty, unsigned long long propertyFlag)
{
	if (num == K)
	{
		if (frequentProperty.find(strProperty) == frequentProperty.end())
		{
			frequentProperty[strProperty].cnt = 1;
			frequentProperty[strProperty].propertyFlag = propertyFlag;
		}
		else
		{
			try
			{
				if (frequentProperty[strProperty].propertyFlag != propertyFlag)
				{
					throw runtime_error("属性集没有按要求顺序输入（或者属性集不相互独立）");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}
			++frequentProperty[strProperty].cnt;
		}
	}
	else
	{
		for (size_t i = pos; i != toMatchProperties.size(); ++i)
		{
			if (num + (toMatchProperties.size() - i) >= K)
			{
				const string &sigleProperty = nd.properties[toMatchProperties[i]];
				if (siglePropertySet.at(sigleProperty).cnt >= minsupp)
				{
					getAllPropertySet(num + 1, i + 1, K, nd, siglePropertySet,
						strProperty + sigleProperty,
						propertyFlag | static_cast<unsigned long long>(1 << toMatchProperties[i]));
				}
			}
			else
			{
				break;
			}
		}
	}
}

/*
	获取所有属性集合出现次数（利用单独属性出现次数，做剪枝，求出可能是频繁子树的属性集合）,
	最终会保存：所有频繁属性集合 + 一些非频繁子树。
	所以，需要移除非频繁属性集合。
*/
inline void removeInfrequentPropertySet()
{
	vector<string> ptyStr_del;
	for (unordered_map<string, PropertyMeg>::iterator it = frequentProperty.begin();
		it != frequentProperty.end(); ++it)
	{
		if ((*it).second.cnt < minsupp)
		{
			ptyStr_del.push_back(it->first);
		}
	}
	for (size_t i = 0; i != ptyStr_del.size(); ++i)
	{
		frequentProperty.erase(ptyStr_del[i]);
	}
}

inline void getAllPropertySetWithCnt_1(const unordered_map<string, PropertyMeg> &siglePropertySet)
{
	for (size_t i = 0; i < Data.size(); ++i)
	{
		for (size_t j = 0; j < Data[i].size(); ++j)
		{
			Node &nd = Data[i][j];
			for (size_t k = 1; k != toMatchProperties.size() + 1; ++k)
			{
				getAllPropertySet(0, 0, k, nd, siglePropertySet, "", 0);
			}
		}
	}
	removeInfrequentPropertySet();
}


/*
	方法二：基于投影分支的思想，直接生成频繁属性集合，但需要多次遍历数据库
	适用于：属性个数较多，直接枚举属性的复杂度大于遍历数据库的复杂度
*/

// 根据propertyFlag（即利用二进制位记录了使用的属性），获得属性集合
inline void getPropertyStr(string &ptyStr, unsigned long long ptyPos, const Node &nd)
{
	unsigned bitNum = 0;
	while (ptyPos)
	{
		if (ptyPos & 1)
		{
			//ptyStr += nd.properties[bitNum];
			ptyStr.append(nd.properties[bitNum]);
		}
		ptyPos >>= 1;
		++bitNum;
	}
}

// 获取一个节点的临近属性
inline void getOneNodeAdjoinProperty(const unsigned long long &propertyPos, const Node &nd,
	unordered_map<string, PropertyMeg> &adjoinProperty)
{
	for (vector<size_t>::const_reverse_iterator cit = toMatchProperties.crbegin();
		cit != toMatchProperties.crend(); ++cit)
	{
		unsigned long long shiftRel = static_cast<unsigned long long>(1 << (*cit));
		if (!(shiftRel & propertyPos))
		{
			const string &pty = nd.properties[*cit];
			if (adjoinProperty.find(pty) == adjoinProperty.end())
			{
				adjoinProperty[pty].cnt = 1;
				adjoinProperty[pty].propertyFlag = shiftRel;
			}
			else
			{
				try
				{
					if (shiftRel != adjoinProperty[pty].propertyFlag)
					{
						throw runtime_error("属性集没有按要求顺序输入（或者属性集不相互独立）");
					}
				}
				catch (runtime_error err)
				{
					cerr << "Error: " << __FILE__
						<< " : in function " << __func__
						<< " at line " << __LINE__ << endl
						<< "     Comiled on " << __DATE__
						<< " at " << __TIME__ << endl
						<< err.what() << endl;
				}
				++adjoinProperty[pty].cnt;
			}
		}
		else
		{
			return;
		}
	}
}

// 获取临近属性
inline void getAdjoinProperty(const unsigned long long &propertyPos, const string &propertyStr,
	unordered_map<string, PropertyMeg> &adjoinProperty)
{
	for (size_t i = 0; i < Data.size(); ++i)
	{
		for (size_t j = 0; j < Data[i].size(); ++j)
		{
			Node &nd = Data[i][j];
			string ptyStr;
			getPropertyStr(ptyStr, propertyPos, nd);
			if (propertyStr == ptyStr)
			{
				getOneNodeAdjoinProperty(propertyPos, nd, adjoinProperty);
			}
		}
	}
}

// 属性扩展
inline void propertyExpand(const unsigned long long &propertyPos, const string &propertyStr,
	unordered_map<string, PropertyMeg> &adjoinProperty)
{
	for (unordered_map<string, PropertyMeg>::const_iterator cit = adjoinProperty.cbegin();
		cit != adjoinProperty.end(); ++cit)
	{
		const PropertyMeg &ptyMeg = (*cit).second;
		if (ptyMeg.cnt >= minsupp)
		{
			string ptyStr = propertyStr + (*cit).first;
			try
			{
				if (frequentProperty.find(ptyStr) != frequentProperty.end())
				{
					throw runtime_error("求取邻近属性出现重复");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}

			frequentProperty[ptyStr].cnt = ptyMeg.cnt;
			frequentProperty[ptyStr].propertyFlag = (propertyPos | ptyMeg.propertyFlag);
			unordered_map<string, PropertyMeg> adjoinPropertyNext;
			const unsigned long long ptyPos = (propertyPos | ptyMeg.propertyFlag);
			getAdjoinProperty(ptyPos, ptyStr, adjoinPropertyNext);
			propertyExpand(ptyPos, ptyStr, adjoinPropertyNext);
		}
	}
}


// 获取所有属性集合出现次数（拓展），并保存所有频繁属性集合
inline void getAllPropertySetWithCnt_2(const unordered_map<string, PropertyMeg> &siglePropertySet)
{
	for (unordered_map<string, PropertyMeg>::const_iterator cit = siglePropertySet.cbegin();
		cit != siglePropertySet.end(); ++cit)
	{
		const PropertyMeg &ptyMeg = (*cit).second;
		if (ptyMeg.cnt >= minsupp)
		{
			try
			{
				if (frequentProperty.find((*cit).first) != frequentProperty.end())
				{
					throw runtime_error("获取的单独属性出现重复");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}

			frequentProperty[(*cit).first] = ptyMeg;
			unordered_map<string, PropertyMeg> adjoinProperty;
			const unsigned long long &propertyPos = ptyMeg.propertyFlag;
			getAdjoinProperty(propertyPos, (*cit).first, adjoinProperty);
			propertyExpand(propertyPos, (*cit).first, adjoinProperty);
		}
	}
}


// 获得所有频繁属性集合（根据具体输入数据选择方法）
inline void getAllFrequentPropertySet()
{
	unordered_map<string, PropertyMeg> siglePropertySet;
	getSinglePropertySetWithCnt(siglePropertySet);


	if (toMatchProperties.size() < 20)
	{
		getAllPropertySetWithCnt_1(siglePropertySet);
	}
	else
	{
		getAllPropertySetWithCnt_2(siglePropertySet);
	}
	//outputFrequentProperty();
}
/********************************************* 获取所有属性集合 ************************************************/



/*************************************** 基于投影分支的多维频繁子树挖掘 ******************************************/

// 标记“模式树”的节点在数据树中所对应的节点，并判断是否全部找到
inline bool signAvailableNode(vector<Node> &Item, const vector<NodeForT> &Ttree,
	unordered_map<string, size_t> &availableNode)
{
	vector<size_t> visitPos;
	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		visitPos.push_back(i);
	}

	for (size_t i = 0; i != Item.size(); ++i)
	{
		Node &nd = Item[i];
		for (size_t j = 0; j != visitPos.size(); ++j)
		{
			string ndPtyStr;
			const NodeForT &ndT = Ttree[visitPos[j]];
			getPropertyStr(ndPtyStr, ndT.propertyFlag, nd);
			if (ndPtyStr == ndT.uuidByMatchProperty)
			{
				nd.uuidByMatchProperty = ndPtyStr;
				availableNode[ndPtyStr] = i;
				visitPos.erase(visitPos.begin() + j);
				if (visitPos.empty())
				{
					return true;
				}
				break;
			}
		}
	}
	return false;
}

// 更新可用点的父节点（即：获得由可用点组成的森林）
inline void updateAvailableNodeFather(vector<Node> &Item,
	const unordered_map<string, size_t> &availableNode)
{
	unordered_set<size_t> visitNodePos;
	for (unordered_map<string, size_t>::const_iterator cit = availableNode.cbegin();
		cit != availableNode.cend(); ++cit)
	{
		visitNodePos.insert((*cit).second);
	}

	for (unordered_set<size_t>::const_iterator cit = visitNodePos.cbegin();
		cit != visitNodePos.cend(); ++cit)
	{
		Node &nd = Item[*cit];
		int father = nd.father;

		while ((father >= 0) &&
			(visitNodePos.find(static_cast<unsigned>(father)) == visitNodePos.end()))
		{
			father = Item[father].father;
		}
		nd.availableFather = father;
	}
}

// 判断模式树与数据树中的“模式树”的父子关系是否一致（直接排除了数据树中的“模式树”是森林的情况）
inline bool judgeRelationshipOfFatherSonEqual(const vector<NodeForT> &Ttree,
	const vector<Node> &Item, const unordered_map<string, size_t> &availableNode)
{
	if (Ttree.size() != availableNode.size())
	{
		return false;
	}

	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		const NodeForT &ndT = Ttree[i];

		if (ndT.father >= 0)
		{
			const string &fatherPtySet = Ttree[ndT.father].uuidByMatchProperty;
			if (availableNode.find(ndT.uuidByMatchProperty) != availableNode.end())
			{
				int ndFather = Item[availableNode.at(ndT.uuidByMatchProperty)].availableFather;
				if (ndFather >= 0)
				{
					if (Item[ndFather].uuidByMatchProperty != fatherPtySet)
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (availableNode.find(ndT.uuidByMatchProperty) != availableNode.end())
			{
				int ndFather = Item[availableNode.at(ndT.uuidByMatchProperty)].availableFather;
				if (ndFather >= 0)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}


// 获得数据树中的“模式树”的节点顺序
inline void getTtreeInItemOrder(const vector<Node> &Item,
	const unordered_map<string, size_t> &availableNode, vector<string> &TtreeInItemOrder)
{
	size_t fatherID = availableNode.at(Troot);
	TtreeInItemOrder.push_back(Troot);

	// 标记数据树中的“模式树”是否访问过
	unordered_map<size_t, bool> nodeNumVisited;
	nodeNumVisited[fatherID] = true;

	// 将数据树中的“模式树”的节点标号从小到大排列
	vector<size_t> availableNodeNum;
	for (unordered_map<string, size_t>::const_iterator cit = availableNode.cbegin();
		cit != availableNode.cend(); ++cit)
	{
		availableNodeNum.push_back((*cit).second);
		nodeNumVisited[(*cit).second] = false;
	}
	sort(availableNodeNum.begin(), availableNodeNum.end());

	// 生成数据树中的“模式树”的序列表示。（生成的排序方式：1.层；2.节点自身ID值）
	queue<size_t> queNode;
	queNode.push(fatherID);

	while (!queNode.empty())
	{
		fatherID = queNode.front();
		queNode.pop();

		for(size_t i = 0; i != availableNodeNum.size(); ++i)
		{
			if (!nodeNumVisited[availableNodeNum[i]])
			{
				const Node &nd = Item[availableNodeNum[i]];
				if (nd.availableFather == fatherID)
				{
					TtreeInItemOrder.push_back(nd.uuidByMatchProperty);
					nodeNumVisited[availableNodeNum[i]] = true;
					queNode.push(availableNodeNum[i]);
				}
			}
		}
	}
}


// 判断模式树与数据树中的“模式树”的节点顺序是否一致
inline bool judgeTreeNodeOrder(const vector<Node> &Item, const vector<NodeForT> &Ttree,
	const unordered_map<string, size_t> &availableNode, const vector<string> &TtreeOrder)
{
	//map<size_t, string> TtreeInItemOrder;
	vector<string> TtreeInItemOrder;
	getTtreeInItemOrder(Item, availableNode, TtreeInItemOrder);


	if (TtreeInItemOrder.size() != TtreeOrder.size())
	{
		return false;
	}

	for (size_t i= 0; i != TtreeInItemOrder.size(); ++i)
	{
		if (TtreeInItemOrder[i] != TtreeOrder[i])
		{
			return false;
		}
	}

	return true;
}

// 判断“模式树”是否存在于数据树中
inline bool judgeTtreeExist(vector<Node> &Item, const vector<NodeForT> &Ttree,
	unordered_map<string, size_t> &availableNode, const vector<string> &TtreeOrder)
{
	// 标记数据树中的“模式树”（标记节点）
	if (signAvailableNode(Item, Ttree, availableNode))
	{
		// 标记数据树中的“模式树”的父节点
		updateAvailableNodeFather(Item, availableNode);

		// 判断模式树与数据树中的“模式树”的父子关系是否一致（直接排除了数据树中的“模式树”是森林的情况）
		if (judgeRelationshipOfFatherSonEqual(Ttree, Item, availableNode))
		{
			// 判断模式树与数据树中的“模式树”的节点顺序是否一致
			if (judgeTreeNodeOrder(Item, Ttree, availableNode, TtreeOrder))
			{
				return true;
			}
		}
	}
	return false;
}

// 记录“模式树”在数据树中的位置
inline void recordTtreePosInItem(const vector<Node> &Item, const size_t nowItemID,
	const unordered_map<string, size_t> &availableNode)
{
	SubTree subTr;
	subTr.itemNo = nowItemID;
	subTr.root = availableNode.at(Troot);
	subTr.rml = availableNode.at(Trml);

	unordered_map<size_t, int> &toFatherID = subTr.toFatherID;

	int fatherID = subTr.rml;
	while (fatherID >= 0)
	{
		toFatherID[static_cast<size_t>(fatherID)] = Item[fatherID].availableFather;
		fatherID = Item[fatherID].availableFather;
	}
	itemToSubTr.push_back(subTr);
}

// 求投影分支
inline void getProjectedBranch(const vector<NodeForT> &Ttree, const vector<string> &TtreeOrder)
{
	for (size_t i = 0; i < visitDataPos.size(); )
	{
		vector<Node> &Item = Data[visitDataPos[i]];

		// 数据树中对应的“模式树”的节点（属性集合--编号，两者均唯一且一一对应）
		unordered_map<string, size_t> availableNode;

		// 判断当前数据树中是否存在“模式树”
		if (judgeTtreeExist(Item, Ttree, availableNode, TtreeOrder))
		{
			// 记录数据树中的“模式树”的位置
			recordTtreePosInItem(Item, visitDataPos[i], availableNode);
			++i;
		}else
		{
			visitDataPos.erase(visitDataPos.begin() + i);
		}
	}
}

// 获得RMB(T)的儿子节点（即：投影节点）关键代码【注：重构，单独提出，目的减少重复代码】
inline void getPBnodeRMBchildCore(vector<size_t> &PBnode,
	const vector<Node> &item, const int nowNode, const int standardNo)
{
	const Node &nd = item[nowNode];
	for (size_t i = 0; i != nd.sonNode.size(); ++i)
	{
		if (-1 != standardNo)
		{
			if (item[nd.sonNode[i]].orderID > standardNo)
			{
				PBnode.push_back(nd.sonNode[i]);
			}
		}
		else
		{
			PBnode.push_back(nd.sonNode[i]);
		}
	}
}


// 获得RMB(T)的儿子节点（即：投影节点的一部分）
inline void getPBNodeRMBChild(unordered_map<string, vector<size_t>> &TnodeToPBnode,
	const vector<Node> &item, const SubTree &subTr, unordered_set<string> &notVisitNode)
{
	int nowNode = subTr.rml;
	int standardNo = -1;
	while (nowNode >= 0)
	{
		// 剪枝1
		//（剪掉【不能产生投影分支频繁节点】的节点）
		/*
			论文::定理3：
				令T为数据集TDB中的频繁子树，V∈RMB(T)。V在TDB的投影分支中没有投影分支频繁节点，
				则模式树T和通过T最右扩展得到的所有子树T'都不可能在节点V向右扩展得到新的频繁子树。
		*/
		const string &nowNodeUuid = item[nowNode].uuidByMatchProperty;
		if (notVisitNode.find(nowNodeUuid) == notVisitNode.end())
		{
			if (TnodeToPBnode.find(nowNodeUuid) == TnodeToPBnode.end())
			{
				vector<size_t> PBnode;
				getPBnodeRMBchildCore(PBnode, item, nowNode, standardNo);
				TnodeToPBnode[nowNodeUuid] = PBnode;
			}
		}

		int nextNode = subTr.toFatherID.at(nowNode);
		if (nextNode >= 0)
		{
			while ((nowNode >= 0) && (nowNode != nextNode))
			{
				standardNo = item[nowNode].orderID;
				nowNode = item[nowNode].father;
			}
		}
		else
		{
			break;
		}
	}
}

// 获得RMB(T)的右兄弟节点（即：投影节点的一部分）
inline void getPBNodeRMBRightBrother(unordered_map<string, vector<size_t>> &TnodeToPBnode,
	const vector<Node> &item, const SubTree &subTr)
{
	int nowNode = subTr.rml;
	int fatherNode = item[nowNode].father;
	int standardNo = item[nowNode].orderID;
	int availableFather = item[nowNode].availableFather;


	while ((fatherNode >= 0) && (availableFather >= 0))
	{
		if(fatherNode != availableFather)
		{
			const string &fatherNodeUuid = item[availableFather].uuidByMatchProperty;
			if (TnodeToPBnode.find(fatherNodeUuid) != TnodeToPBnode.end())
			{
				vector<size_t> &PBnode = TnodeToPBnode[fatherNodeUuid];
				getPBnodeRMBchildCore(PBnode, item, fatherNode, standardNo);
			}
		}

		nowNode = item[nowNode].father;
		if (nowNode >= 0)
		{
			fatherNode = item[nowNode].father;
			standardNo = item[nowNode].orderID;

			if (subTr.toFatherID.find(nowNode) != subTr.toFatherID.end())
			{
				availableFather = item[nowNode].availableFather;
			}
		}
		else
		{
			break;
		}
	}
}

// 获取一个节点的所有属性集合
inline void getOneNodePtySet(size_t num, size_t pos, const
	size_t &K, const Node &nd,
	string strProperty, unsigned long long propertyFlag,
	unordered_map<string, PropertyMeg> &PBnodeToMeg)
{
	if (num == K)
	{
		if (frequentProperty.find(strProperty) != frequentProperty.end())
		{
			if (PBnodeToMeg.find(strProperty) != PBnodeToMeg.end())
			{
				try
				{
					if (PBnodeToMeg[strProperty].propertyFlag != propertyFlag)
					{
						throw runtime_error("属性集没有按要求顺序输入（或者属性集不相互独立）");
					}
				}
				catch (runtime_error err)
				{
					cerr << "Error: " << __FILE__
						<< " : in function " << __func__
						<< " at line " << __LINE__ << endl
						<< "     Comiled on " << __DATE__
						<< " at " << __TIME__ << endl
						<< err.what() << endl;
				}
				++PBnodeToMeg[strProperty].cnt;

			}
			else
			{
				PBnodeToMeg[strProperty].cnt = 1;
				PBnodeToMeg[strProperty].propertyFlag = propertyFlag;
			}
		}
	}
	else
	{
		for (size_t i = pos; i != toMatchProperties.size(); ++i)
		{
			if (num + (toMatchProperties.size() - i) >= K)
			{
				const string &sigleProperty = nd.properties[toMatchProperties[i]];
				if (frequentProperty.find(sigleProperty) != frequentProperty.end())
				{
					getOneNodePtySet(num + 1, i + 1, K, nd,
						strProperty + sigleProperty,
						propertyFlag | static_cast<unsigned long long>(1 << toMatchProperties[i]),
						PBnodeToMeg);
				}
			}
			else
			{
				break;
			}
		}
	}
}

// 获得RMB(T)的儿子节点和右兄弟节点的后代节点（即：投影节点的一部分）
/*
至此，即可获得所有投影分支节点
*/
inline void getPBNodeRMBGrandson(
	unordered_map<string, unordered_map<string, PropertyMeg>> &TnodeToPBnodeCnt,
	const unordered_map<string, vector<size_t>> &TnodeToPBnode, const vector<Node> &item)
{
	for (unordered_map<string, vector<size_t>>::const_iterator cit = TnodeToPBnode.cbegin();
		cit != TnodeToPBnode.cend(); ++cit)
	{
		if (!(*cit).second.empty())
		{
			const vector<size_t> &PBnode = (*cit).second;
			queue<size_t> toVisited;
			for (size_t i = 0; i != PBnode.size(); ++i)
			{
				toVisited.push(PBnode[i]);
			}

			unordered_map<string, PropertyMeg> PBnodeToMeg;
			while (!toVisited.empty())
			{
				const Node &nd = item[toVisited.front()];
				toVisited.pop();
				for (size_t k = 1; k != toMatchProperties.size() + 1; ++k)
				{
					getOneNodePtySet(0, 0, k, nd, "", 0, PBnodeToMeg);
				}
				for (size_t i = 0; i != nd.sonNode.size(); ++i)
				{
					toVisited.push(nd.sonNode[i]);
				}
			}
			// 统计投影节点
			if (TnodeToPBnodeCnt.find((*cit).first) != TnodeToPBnodeCnt.end())
			{
				unordered_map<string, PropertyMeg> &pbToMeg_Tmp = TnodeToPBnodeCnt.at((*cit).first);
				for (unordered_map<string, PropertyMeg>::const_iterator cit_PBToMeg = PBnodeToMeg.cbegin();
					cit_PBToMeg != PBnodeToMeg.cend(); ++cit_PBToMeg)
				{
					if (pbToMeg_Tmp.find((*cit_PBToMeg).first) != pbToMeg_Tmp.end())
					{
						pbToMeg_Tmp[(*cit_PBToMeg).first].cnt += (*cit_PBToMeg).second.cnt;
					}
					else
					{
						pbToMeg_Tmp[(*cit_PBToMeg).first] = (*cit_PBToMeg).second;
					}
				}
			}
			else
			{
				TnodeToPBnodeCnt[(*cit).first] = PBnodeToMeg;
			}
		}
	}

}

// 获得投影节点出现次数
inline void getPBnodeCnt(unordered_map<string, unordered_map<string, PropertyMeg>> &TnodeToPBnodeCnt,
	const vector<Node> &item, const SubTree &subTr, unordered_set<string> &notVisitNode)
{
	unordered_map<string, vector<size_t>> TnodeToPBnode;
	getPBNodeRMBChild(TnodeToPBnode, item, subTr, notVisitNode);
	getPBNodeRMBRightBrother(TnodeToPBnode, item, subTr);
	getPBNodeRMBGrandson(TnodeToPBnodeCnt, TnodeToPBnode, item);
}

// 获得模式树节点元素唯一标识
inline void getTtreeNodeUuid(const vector<NodeForT> &Ttree, unordered_map<string, size_t> &TtreeNodeUuid)
{
	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		TtreeNodeUuid[Ttree[i].uuidByMatchProperty] = i;
	}
}

// 获得“模式树”的节点顺序以及最右节点
inline void getTtreeOrder(const vector<NodeForT> &Ttree, vector<string> &TtreeOrder)
{

	vector<bool> visited(Ttree.size());
	fill(visited.begin(), visited.end(), false);

	size_t fatherID = 0;
	for (size_t i = 0; i != Ttree.size(); ++i)
	{
		if (Ttree[i].father < 0)
		{
			TtreeOrder.push_back(Ttree[i].uuidByMatchProperty);
			visited[i] = true;
			fatherID = Ttree[i].id;
			break;
		}
	}

	queue<size_t> queNode;
	queNode.push(fatherID);

	int rmlNode = fatherID;

	while (!queNode.empty())
	{
		fatherID = queNode.front();
		queNode.pop();
		int rmlTmp = -1;
		for (size_t i = 0; i != Ttree.size(); ++i)
		{
			if (!visited[i])
			{
				const NodeForT &ndT = Ttree[i];
				if (ndT.father == fatherID)
				{
					TtreeOrder.push_back(ndT.uuidByMatchProperty);
					visited[i] = true;
					queNode.push(ndT.id);

					if (rmlNode == fatherID)
					{
						rmlTmp = ndT.id;
					}
				}
			}
		}
		if (-1 != rmlTmp)
		{
			rmlNode = rmlTmp;
		}
	}
	Trml = Ttree[rmlNode].uuidByMatchProperty;
}

// 当前子树根据投影分支进行扩展
inline void subtreeExpandBasedOnProjectedBranch(vector<NodeForT> &Ttree,
	unordered_set<string> notVisitNodePre)
{
	if (!visitDataPos.empty())
	{
		unordered_map<string, unordered_map<string, PropertyMeg>> TnodeToPBnodeCnt;

		for (size_t i = 0; i != itemToSubTr.size(); ++i)
		{
			SubTree &subTr = itemToSubTr[i];
			vector<Node> &item = Data[subTr.itemNo];
			// 求在当前包含模式树的数据树上，所包含的投影分支上相同节点出现的次数
			getPBnodeCnt(TnodeToPBnodeCnt, item, subTr, notVisitNodePre);
		}

		unordered_map<string, size_t> TtreeNodeUuid;
		getTtreeNodeUuid(Ttree, TtreeNodeUuid);


		unordered_set<string> notVisitNode(notVisitNodePre);	// 不需要访问的节点（论文定理3，剪枝）
		for (unordered_map<string, unordered_map<string, PropertyMeg>>::const_iterator cit = TnodeToPBnodeCnt.cbegin();
			cit != TnodeToPBnodeCnt.cend(); ++cit)
		{
			bool flag = false;
			const unordered_map<string, PropertyMeg> &PBnodeToMeg = (*cit).second;
			for (unordered_map<string, PropertyMeg>::const_iterator cit_pbToMeg = PBnodeToMeg.cbegin();
				cit_pbToMeg != PBnodeToMeg.cend(); ++cit_pbToMeg)
			{
				if (TtreeNodeUuid.find((*cit_pbToMeg).first) == TtreeNodeUuid.end() &&
					(*cit_pbToMeg).second.cnt >= minsupp)
				{
					flag = true;
					break;
				}
			}
			if (!flag)
			{
				notVisitNode.insert(cit->first);
			}
		}


		for (unordered_map<string, unordered_map<string, PropertyMeg>>::const_iterator cit = TnodeToPBnodeCnt.cbegin();
			cit != TnodeToPBnodeCnt.cend(); ++cit)
		{
			if (notVisitNode.find(cit->first) != notVisitNode.end())
			{
				continue;
			}


			try
			{
				if (TtreeNodeUuid.find((*cit).first) == TtreeNodeUuid.end())
				{
					throw runtime_error("没有找到投影节点的父节点");
				}
			}
			catch (runtime_error err)
			{
				cerr << "Error: " << __FILE__
					<< " : in function " << __func__
					<< " at line " << __LINE__ << endl
					<< "     Comiled on " << __DATE__
					<< " at " << __TIME__ << endl
					<< err.what() << endl;
			}

			size_t pbNodeFatherPos = TtreeNodeUuid[(*cit).first];

			const unordered_map<string, PropertyMeg> &PBnodeToMeg = (*cit).second;

			for (unordered_map<string, PropertyMeg>::const_iterator cit_pbToMeg = PBnodeToMeg.cbegin();
				cit_pbToMeg != PBnodeToMeg.cend(); ++cit_pbToMeg)
			{
				if (TtreeNodeUuid.find((*cit_pbToMeg).first) == TtreeNodeUuid.end() &&
					(*cit_pbToMeg).second.cnt >= minsupp)
				{
					NodeForT ndT;
					ndT.father = pbNodeFatherPos;
					ndT.id = TtreeMeg.nodeCnt;
					ndT.propertyFlag = (*cit_pbToMeg).second.propertyFlag;
					ndT.uuidByMatchProperty = (*cit_pbToMeg).first;

					++TtreeMeg.nodeCnt;


					Ttree.push_back(ndT);

					outputFrequentSubtree(Ttree);
					//calDataNum(frequentSubtreeNum);

					if (!itemToSubTr.empty())
					{
						itemToSubTr.clear();
					}

					vector<string> TtreeOrder;

					string trmlTmp = Trml;

					// 获得“模式树”的节点顺序以及最右节点
					getTtreeOrder(Ttree, TtreeOrder);

					vector<size_t> vtDtPs_Tmp(visitDataPos.begin(), visitDataPos.end());

					// 求投影分支
					getProjectedBranch(Ttree, TtreeOrder);

					// 当前子树根据投影分支进行扩展
					subtreeExpandBasedOnProjectedBranch(Ttree, notVisitNode);

					if (vtDtPs_Tmp.size() != visitDataPos.size())
					{
						visitDataPos.swap(vtDtPs_Tmp);
					}

					Trml = trmlTmp;

					--TtreeMeg.nodeCnt;
					Ttree.erase(Ttree.end() - 1);
				}
			}
		}
	}
}



// 基于投影分支的多维频繁子树挖掘算法
inline void frequentMulpropertySubtreeMiningBasedOnProjectedBranch()
{
	for (unordered_map<string, PropertyMeg>::const_iterator cit = frequentProperty.cbegin();
		cit != frequentProperty.cend(); ++cit)
	{
		vector<NodeForT> Ttree;

		TtreeMeg.nodeCnt = 0;

		NodeForT tnd;
		tnd.father = -1;
		tnd.id = TtreeMeg.nodeCnt;
		tnd.propertyFlag = (*cit).second.propertyFlag;
		tnd.uuidByMatchProperty = (*cit).first;

		++TtreeMeg.nodeCnt;

		Ttree.push_back(tnd);

		outputFrequentSubtree(Ttree);
		//calDataNum(frequentSubtreeNum);


		if (!itemToSubTr.empty())
		{
			itemToSubTr.clear();
		}

		vector<string> TtreeOrder;
		// 获得“模式树”的节点顺序
		getTtreeOrder(Ttree, TtreeOrder);


		// 获得“模式树”的根节点与最右叶节点
		Troot = (*cit).first;
		Trml = (*cit).first;

		// 初始化要访问的数据树下标
		if (!visitDataPos.empty())
		{
			visitDataPos.clear();
		}
		for (size_t i = 0; i != Data.size(); ++i)
		{
			visitDataPos.push_back(i);
		}

		// 求投影分支
		getProjectedBranch(Ttree, TtreeOrder);
		unordered_set<string> notVisitNode;

		// 当前子树根据投影分支进行扩展
		subtreeExpandBasedOnProjectedBranch(Ttree, notVisitNode);
	}
}

/*************************************** 基于投影分支的多维频繁子树挖掘 ******************************************/

int main()
{
#ifdef HOME
	time_t startTime, endTime;
	startTime = clock();

	std::freopen("200.txt", "r", stdin);
	std::freopen("200-2-2", "w", stdout);

	//std::freopen("in", "r", stdin);
	//std::freopen("out", "w", stdout);

#endif // HOME

	setPropertyNum();
	setToMatchProperties();
	setMinsupp();
	inputData();
	setDataTreeOrderID();

	getAllFrequentPropertySet();

	frequentMulpropertySubtreeMiningBasedOnProjectedBranch();

	cout << "多维频繁子树的个数是：" << frequentSubtreeNum << endl;

#ifdef HOME
	//std::ios::sync_with_stdio(false);
	endTime = clock();
	cout << "Time elapsed: " << (endTime - startTime) << "ms" << endl;
	cout << "Time elapsed: " << clock() / CLOCKS_PER_SEC << "s" << endl;
	fclose(stdout);
	fclose(stdin);
	//fout.close();
	//fin.close();
#endif // HOME

}
