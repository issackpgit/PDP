/*  ISSAC KOSHY
 *  PANICKER
 *  issackos
 */

#ifndef A0_HPP
#define A0_HPP

template <typename T, typename Op>
void omp_scan(int n, const T* in, T* out, Op op) {
    if(n==1)
        return;
    
    int * S = (int*) malloc ((n/2+1)*sizeof(int));
    int * x = (int*) malloc ((n+1)*sizeof(int));
    int i = 0;
    S[n/2] = 0;
    x[n] = 0;
    x = const_cast<int*>(in);
    
    omp_set_num_threads(8);
    
#pragma omp parallel for
    for (i = 0; i < n/2; ++i)
    {
        S[i] = op(x[2*i],x[op(2*i,1)]);
    }
    
    omp_scan(n/2, S, out, std::plus<int>());
    
#pragma omp parallel for
    for (i = 1; i < n; i++)
    {
        if(i%2==0){
            x[i] = op(S[(i/2)-1], x[i]);
        }
        else{
            x[i] = S[i/2];
        }
    }
    
    for(int i =0;i<n;i++)
    {
        out[i]=x[i];
    }
}// omp_scan

#endif // A0_HPP
