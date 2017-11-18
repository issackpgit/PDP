/*  Issac Koshy
 *  Panicker
 *  issackos
 */

#ifndef A1_HPP
#define A1_HPP

#include <vector>
#include <mpi.h>
#include <unordered_set>

void display(std::vector<signed char>& V, int b, int rank){
    std::cout<<"Rank in hpp:"<<rank<<std::endl;
    for (int i = 0; i < b; ++i) {
        for (int j = 0; j < b; ++j) std::cout << static_cast<int>(V[i * b + j]) << " ";
        std::cout << std::endl;
    }
}

int connected_components(std::vector<signed char>& A, int n, int q, const char* out, MPI_Comm comm) {
    // ...
    int rank,rank1,rank2;
    int i,j;
    MPI_Comm_rank(comm, &rank);
    
    int b = n / q;
    
//------------------------ 1. Initialise Step --------------------
    
    std::vector<signed char> P;
    std::vector<signed char> M;
    M=A;
    P=A;
    
    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            if(P[i * b + j]==1){
                if(rank>=(b/2))
                   P[i * b + j]=i+b;
                else
                   P[i * b + j]=i;
            }
        }
    }
    
//   display(A,b,rank);

    std::vector<int> max(b, 0);
    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            if(P[j*b+i]>max[i])
            max[i] = P[j*b+i];
        }
    }
    
//    display(P,b,rank);

    MPI_Barrier(comm);

    std::vector<int> maxr(b, 0);

    MPI_Comm comm1;
    int color =rank%2;
    MPI_Comm_split(comm, color, rank, &comm1);
    MPI_Comm_rank(comm1, &rank1);
    
// Column wise all reduce(max)
    for(i = 0;i<b;i++){
       MPI_Allreduce(&max[i], &maxr[i], 1, MPI_INT, MPI_MAX,comm1);
    }
    
//    for(i = 0;i<b;i++){
//       std::cout<<"Max new: "<<maxr[i]<<"with rank "<<rank1<<" and old rank "<<rank<<std::endl;
//    }
    
    if(rank1<b/2){
        for (i=0;i<b;i++){
            for(j=0;j<b;j++){
                P[j*b+i]=maxr[i];
            }
        }
    }
    
//    display(P,b,rank);
    
// -------------------- 2. Opportunistic Pointer Jumping --------------------

    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            if(M[i * b + j]==1){
                M[i * b + j]=P[i * b + j];
            }
        }
    }
    
//    display(M,b,rank);
    
    std::vector<int> max1(b, 0);
    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            if(M[i*b+j]>max1[i])
                max1[i] = M[i*b+j];
        }
//        std::cout<<"Max row wise:"<<max1[i]<<std::endl;
    }
    
    MPI_Comm comm2;
    int color1 =rank<2;
    MPI_Comm_split(comm, color1, rank, &comm2);
    MPI_Comm_rank(comm2, &rank2);
    
    std::vector<int> maxr1(b, 0);

    // Row wise all reduce(max)
    for(i = 0;i<b;i++){
        MPI_Allreduce(&max1[i], &maxr1[i], 1, MPI_INT, MPI_MAX,comm2);
    }

    std::vector<signed char> Q;
    Q=M;

    if(rank2<b/2){
        for (i=0;i<b;i++){
            for(j=0;j<b;j++){
                Q[i*b+j]=maxr1[i];
            }
        }
    }
    
//    display(Q,b,rank);

    for(i = 0; i<b;i++){
        for(j = 0;j<b;j++){
            if(rank%2==0){
                if(Q[i*b+j]==j){
                   M[i*b+j]=P[i*b+j];
                }
            }else{
                if(Q[i*b+j]==(j+b)){
                    M[i*b+j]=P[i*b+j];
                }
            }
        }
    }
//    MPI_Barrier(comm);
    
//   display(M,b,rank);
    
    std::vector<int> max2(b, 0);
    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            if(M[i*b+j]>max2[i])
                max2[i] = M[i*b+j];
        }
//                std::cout<<"Max row wise:"<<max2[i]<<std::endl;
    }
    
    std::vector<int> pdash(b, 0);

    // Row wise all reduce(max)
    for(i = 0;i<b;i++){
        MPI_Allreduce(&max2[i], &pdash[i], 1, MPI_INT, MPI_MAX,comm2);
    }
    
    std::vector<signed char> Pp;
    Pp=M;

    if(rank2<b/2){
        for (i=0;i<b;i++){
            for(j=0;j<b;j++){
                Pp[i*b+j]=pdash[i];
            }
        }
    }
    
//    display(Pp,b,rank);

// ------------------------- 3. Tree Hanging ------------------------
    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            M[i*b+j] = 0;
        }
    }
    
    MPI_Barrier(comm);
    
    for(i = 0; i<b;i++){
        for(j = 0;j<b;j++){
            if(rank<b/2){
                if(P[i*b+j]==i){
                    M[i*b+j]=Pp[i*b+j];
                }
            }else{
                if(P[i*b+j]==(i+b)){
                    M[i*b+j]=Pp[i*b+j];
                }
            }
        }
    }
    
//   display(M,b,rank);
    
    std::vector<int> max3(b, 0);
    for (i=0;i<b;i++){
        for(j=0;j<b;j++){
            if(M[i*b+j]>max3[i])
                max3[i] = M[i*b+j];
        }
//                std::cout<<"Max row wise:"<<max3[i]<<std::endl;
    }

    std::vector<int> max4(b, 0);
    
    // Row wise all reduce(max)
    for(i = 0;i<b;i++){
        MPI_Allreduce(&max3[i], &max4[i], 1, MPI_INT, MPI_MAX,comm2);
    }

//        for(i = 0;i<b;i++){
//           std::cout<<"Max new: "<<max4[i]<<"with rank "<<rank2<<" and old rank "<<rank<<std::endl;
//        }
    

    if(rank2<b/2){
        for (i=0;i<b;i++){
            for(j=0;j<b;j++){
                Q[i*b+j]=max4[i];
            }
        }
    }
 //MPI Reduce on P' and Q
    for(i = 0;i<b;i++){
        MPI_Reduce(&pdash[i], &max4[i], 1, MPI_INT, MPI_MAX,0,comm2);
    }
//    for(i = 0;i<b;i++){
//                   std::cout<<"Max new: "<<pdash[i]<<" with rank "<<rank2<<" and old rank "<<rank<<std::endl;
//                }
    
    if(rank2<b/2){
        for (i=0;i<b;i++){
            for(j=0;j<b;j++){
                P[i*b+j]=pdash[i];
            }
        }
    }
    
//    display(P,b,rank);
    
     MPI_Barrier(comm);
    
    std::vector<int> pdashs(b, 0);
    
    int a[2*b];

    for(i =0;i<b;i++){
        if(rank2==0){
            a[i]=pdash[i];
//            std::cout<<"a["<<i<<"] :"<<a[i]<<std::endl;
        }
        else if(rank2==1){
            a[i+b]=pdash[i];
//            std::cout<<"a["<<i+b<<"] :"<<a[i+b]<<std::endl;
        }
    }
    MPI_Barrier(comm);
    
//-------------------------- 4. Finding # of CC ------------------------
    
    int *s;
    int cc=0,comm1size;
    MPI_Comm_size(comm1, &comm1size);
    
    s = (int *)malloc(comm1size*b*sizeof(int));

    MPI_Gather(&a, 4, MPI_INT, s, 4, MPI_INT, 0, comm1);

//    if(rank==0){
//    for(i = 0;i<8;i++){
//        std::cout<<"s["<<i<<"]="<<s[i]<<std::endl;
//    }
//    }
    
    MPI_Barrier(comm);
   
    if(rank==0){
        int len = 2*b;
        std::unordered_set<int> ss(s, s + len);
        cc = ss.size();
    }
    
    MPI_Comm_free(&comm1);
    MPI_Comm_free(&comm2);
    
 return cc;
    
} // connected_components

#endif // A1_HPP
