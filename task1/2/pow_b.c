int pow_b(int a, int b)
{
    int i, res = a;
    for(i = 1; i < b; ++i){
        res *= a;
    }
    
    return res;
}
