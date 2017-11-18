/*  ISSAC KOSHY
 *  PANICKER
 *  issackos
 */

#ifndef A0_HPP
#define A0_HPP

template <typename T, typename Op>
void omp_scan(int n, T* in, T* out, Op op) {
    int *temp;
//    int *x;// = (int*) malloc ((n+1)*sizeof(int));
    int numofthreads, work;
    int i, tid, last;
    int start ,end;
//    x = const_cast<int*>(in);
    
#pragma omp parallel shared(in, out, temp, numofthreads, work, n) private(i, tid, start, end)
    {
#pragma omp single
        {
            numofthreads = omp_get_num_threads();
            work = n / numofthreads + 1;
        }
        tid = omp_get_thread_num();
        
        start = work*tid;
        end =op(start,work);
        if (end>n)
            end=n;
        for(i=start+1;i<end;i++){
            in[i] = op(in[i],in[i - 1]);
        }
        
        out[tid] = in[i-1];
//        printf("Firstout[%d]=%d\n",tid,out[tid]);
//        x[tid]=out[tid];
#pragma omp barrier
        for(i = 1; i < numofthreads; i=2*i) {
//            printf("i is %d \n",i);
//            printf("Out[%d]=%d,Out[%d]=%d\n",tid,out[tid],tid-1,out[tid-1]);
            if(tid >= i)
                temp[tid] = op(out[tid], out[tid-i]);
//            printf("temp[%d] is %d when i is %d and Out[%d]=%d,Out[%d]=%d\n",tid,temp[tid],i,tid,out[tid],tid-1,out[tid-1]);
#pragma omp barrier
//            for(i=0;i<4;i++)
//            {
//                out[i+1]=temp[i+1];
//            }
          std::copy_n(temp+1, numofthreads-1, out+1);
//            printf("Secondout[%d]=%d\n",tid,out[tid]);
        }
        if(end>n)
            end=n;
        for(i = start; i < end; i++)
            in[i] += out[tid]-in[end-1];
    }
    for(i = 0; i < n; i++){
       // printf("%d\n", in[i]);
        out[i]=in[i];
//        printf(" %d ",out[i]);
    }
    
} // omp_scan

#endif // A0_HPP
