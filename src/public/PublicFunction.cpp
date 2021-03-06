#include "PublicFunction.h"
#include<cstdlib>
#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>
#include<sstream>
#include<iostream>
#include<limits>
#include<map>
#include<cmath>
#include<ctime>
#include<string>
#include<functional>
#include<cstddef>
#include "../baseclass/Matrix.h"
#include "../algorithm/ALNS.h"

bool ascendSortForCustId(Customer* item1, Customer* item2) {
    return item1->id < item2->id;
}

float random(float start, float end){
    // 产生start到end之间的随机数
    // some times using (int)output will get the boundary value "end" !!
    return (float)(start+(end-start)*rand()/(RAND_MAX+1.0));
}

vector<float> randomVec(int num){  
    // 产生num个随机数，它们的和为1
    float rest = 1;  // 初始余量为1
    vector<float> output;
    if(num == 1) {
        output.push_back(1);
    } else {
        for(int i=0; i<num - 1; i++) {
            float temp = random(0, rest); // 产生随机数
            output.push_back(temp);
            rest -= temp;
        }
        output.push_back(rest);
    }
    random_shuffle(output.begin(), output.end()); 
    return output;
}

float dist(Customer *p1, Customer *p2) {
    return sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2));
}

vector<int> getRandom(int lb, int ub, int m, vector<int> &restData){
    // 产生m个不同的，范围为[lb, ub)的随机数
    // restData, 除了返回值外剩余的数值
    restData.resize(0);
    for(int i=0; i<ub-lb; i++) {
        restData.push_back(i+lb);
    }
    int total = ub-lb;
    vector<int> outputData(0);
    for(int j=0; j<m; j++) {
        vector<int>::iterator iter = restData.begin();
        int num = (int)rand() % total; // 0-total-1, if normal 
        num = min(num, total - 1);     // avoid exceeding the right side
        iter += num;
        int temp = *(iter);
        outputData.push_back(temp);
        restData.erase(iter);
        total--;
    }
    return outputData;
}

int roulette(vector<float> probability) {
    // 轮盘算法
    // 随机选出一个整数k (from 0 to |probability|)。
    vector<float>::iterator floatIter;
    float sumation1 = accumulate(probability.begin(), probability.end(), 0.0f); // 求和
    for(floatIter = probability.begin(); floatIter < probability.end(); floatIter++) {
        *floatIter /= sumation1;  // 归一化
    }
    int totalNum = probability.end() - probability.begin();  // 总数目
    int k = 0;
    float sumation = 0;
    float randFloat = rand()/(RAND_MAX + 1.0f);
    floatIter = probability.begin();
    while((sumation < randFloat) && (floatIter < probability.end())) {
        k++;
        sumation += *(floatIter++);
    }
    k = min(k-1, totalNum-1); // avoid k is larger than totalNum
    k = max(k, 0);            // when randFloat = 0, here k < 0 will happen
    return k;
}

int roulette(float *probability, int num) {
    // 轮盘算法
    // 随机选出一个整数k (from 0 to |probability|)。
    // 一共有num个概率分布
    int i;
    float sumation1 = 0; // 求和
    for(i=0; i<num; i++) {
        sumation1 += probability[i];
    }
    for(i=0; i<num; i++) {
        probability[i] /= sumation1;  // 归一化
    }
    int k = 0;
    float sumation = 0;
    float randFloat = rand()/(RAND_MAX + 1.0f);
    while((sumation < randFloat) && (k < num)) {
        k++;
        sumation += probability[k];
    }
    k = min(k - 1, num - 1);   // avoid k is larger than num
    k = max(k, 0);             // when randFloat = 0, here k < 0 will happen
    return k;
}

void computeBest(vector<Car*> carSet, vector<Car*> &bestRoute, float &bestCost){
    // 计算对CarSet中顾客的最佳服务路线
    // Args:
    //   * carSet: 可能是一个比较粗糙的解，含有所有顾客节点的信息
    // Returns:
    //   * bestRoute: 最佳服务路线，由ALNS求得
    //   * bestCost: 最小代价
    Customer depot = carSet[0]->getRearNode();
    float capacity = carSet[0]->getCapacity();
    vector<Customer*> allCustomer;
    vector<Car*>::iterator carIter;
    vector<Customer*>::iterator custIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
        vector<Customer*> tempCust = (*carIter)->getAllCustomer();
        cout << "Loading the " << carIter - carSet.begin() << "car data!" << endl;
        for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
            allCustomer.push_back(*custIter);
        }
    }
    depot.priority = 0;
    depot.startTime = 0;
    depot.serviceTime = 0;
    depot.arrivedTime = 0;
    depot.prop = 0;
    ALNS alg(allCustomer, depot, capacity, 25000);
    alg.run(bestRoute, bestCost);
}


int getCustomerNum(vector<Car*> originCarSet) {
    // 获得路径集中顾客节点的数目
    int customerNum = 0;
    for (int i = 0; i<(int)originCarSet.size(); i++) {
        customerNum += originCarSet[i]->getRoute().getSize();
    }
    return customerNum;
}

bool carSetEqual(vector<Car*> carSet1, vector<Car*> carSet2){
    // 判断carSet1和carSet2是否一样(根据其所有的顾客id顺序判断)
    if(carSet1.size() != carSet2.size()) {return false;}
    bool mark = true;
    for(int i=0; i<(int)carSet1.size(); i++){
        vector<Customer*> cust1 = carSet1[i]->getRoute().getAllCustomer();
        vector<Customer*> cust2 = carSet2[i]->getRoute().getAllCustomer();
        if(cust1.size() != cust2.size()) {mark = false; break;}
        for(int j=0; j<(int)cust1.size(); j++) {
            if(cust1[j]->id != cust2[j]->id) {
                mark = false; 
                break;
            }
        }
        deleteCustomerSet(cust1);
        deleteCustomerSet(cust2);
    }
    return mark;
}

bool customerSetEqual(vector<Customer*>c1, vector<Customer*>c2){
    // 判断customer集合c1与c2是否一样（根据id判断）
    if(c1.size() != c2.size()) {return false;}
    bool mark = true;
    for(int i=0; i<(int)c1.size(); i++) {
        if(c1[i]->id != c2[i]->id) {
            mark = false; 
            break;
        }
    }
    return mark;

}

vector<Customer*> extractCustomer(vector<Car*> plan) {
    // 将plan中的顾客节点全部抽取出来
    vector<Customer*> allCustomer;
    vector<Customer*>::iterator custIter;
    vector<Car*>::iterator carIter;
    for (carIter=plan.begin(); carIter<plan.end(); carIter++) {
        vector<Customer*> temp = (*carIter)->getAllCustomer();
        for (custIter=temp.begin(); custIter<temp.end(); custIter++) {
            allCustomer.push_back(*custIter);
        }
    }
    return allCustomer;
}

vector<Customer*> mergeCustomer(vector<Customer*> waitCustomer, vector<Customer*> originCustomer) {
    // 将waitCustomer和originCustomer融合为一个数组
    vector<Customer*> allCustomer;
    vector<Customer*>::iterator custIter;
    for(custIter = waitCustomer.begin(); custIter < waitCustomer.end(); custIter++) {
        allCustomer.push_back(new Customer(**custIter));
    }
    for(custIter = originCustomer.begin(); custIter < originCustomer.end(); custIter++) {
        allCustomer.push_back(new Customer(**custIter));
    }
    return allCustomer;
}

vector<int> getID(vector<Customer*> customerSet){
    // 得到customerSet的所有ID
    vector<int> ids(0);
    ids.reserve(customerSet.end() - customerSet.begin());
    vector<Customer*>::iterator iter = customerSet.begin();
    for(iter; iter<customerSet.end(); iter++){
        ids.push_back((*iter)->id);
    }
    sort(ids.begin(), ids.end());
    return ids;
}

vector<int> getID(vector<Car*> carSet) {
    vector<int> ids(0);
    vector<Car*>::iterator carIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
        vector<Customer*> tempCust = (*carIter)->getAllCustomer();
        vector<int> currentIDs = getID(tempCust);
        ids.insert(ids.end(), currentIDs.begin(), currentIDs.end());
        deleteCustomerSet(tempCust);
    }
    sort(ids.begin(), ids.end());
    return ids;
}

void showAllCustomerID(vector<Car*> carSet) {
    // 显示carSet中所有顾客的id信息
    vector<int> allCustomerId = getID(carSet);
    sort(allCustomerId.begin(), allCustomerId.end());
    vector<int>::iterator intIter;
    int count = 0;
    for(intIter = allCustomerId.begin(); intIter < allCustomerId.end(); intIter++) {
        if (count % 8 == 0) {
            cout << endl;
        }
        cout << (*intIter) << '\t';
        count++;
    }
    cout << endl;
}

void showAllCustomerID(vector<Customer*> customerSet) {
    vector<int> allCustomerId = getID(customerSet);
    sort(allCustomerId.begin(), allCustomerId.end());
    vector<int>::iterator intIter;
    int count = 0;
    for(intIter = allCustomerId.begin(); intIter < allCustomerId.end(); intIter++) {
        if (count % 8 == 0) {
            cout << endl;
        }
        cout << (*intIter) << '\t';
        count++;
    }
    cout << endl;
}

void showDetailForPlan(vector<Car*> carSet) {
    // 展示carSet中每一条路径的具体信息
    // 包括顾客顺序，以及每个顾客的位置，时间窗，到达时间等
    vector<Car*>::iterator carIter;
    vector<Customer*>::iterator custIter;
    for(carIter = carSet.begin(); carIter < carSet.end(); carIter++) {
        int index = carIter - carSet.begin();
        cout << "----------------------" << endl;
        cout << "Route " << index << ":" << endl;
        cout << "Depot: x-" << 40 << "\t" << " y-" << 50 << endl;
        vector<Customer*> tempCust = (*carIter)->getAllCustomer();
        for(custIter = tempCust.begin(); custIter < tempCust.end(); custIter++) {
            cout << "Customer #" << custIter-tempCust.begin() << ": x-" << (*custIter)->x << "\t" <<
                "y-" << (*custIter)->y << "\t" << "AT-" << (*custIter)->arrivedTime << "\t" << "ST-" <<
                (*custIter)->startTime << "\t" << "ET-" << (*custIter)->endTime << endl;
        }
        cout << "Depot: x-" << 40 << "\t" <<  "y-" << 50 << endl;
    }
}
