int pow_n(int a, int n)
{
    int res = a;
    for(int i = 0; i < n-1; ++i){
        res *=a;
    }
    
    return res;
}
