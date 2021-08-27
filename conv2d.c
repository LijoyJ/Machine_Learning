#include <stdio.h>
#include <malloc.h>
int matrix_multiply(int **mat1,int **mat2,int **res,int input_M,int input_N,int filter_M,int filter_N)
{
    printf("------------------- multiply --------------------\n");
    int sum = 0;
    for (int i = 0; i < input_M; i++) {
        for (int j = 0; j < filter_N; j++) {
            res[i][j] = 0;
            for (int k = 0; k < filter_M; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
                sum += res[i][j];
        }
    }
    return sum;
}

int conv_matrix_multiply(int **mat1,int **mat2,int input_M,int input_N,
                            int filter_M,int filter_N,int stride_pos_H,int stride_pos_W)
{
    //printf("------------------- multiply --------------------\n");
    int sum = 0;
    for (int i = 0; i < filter_M; i++) {
        for (int j = 0; j < filter_N; j++) {
            for (int k = 0; k < filter_M; k++)
            {
                sum += mat1[i+stride_pos_H][k + stride_pos_W ] * mat2[k][j];
                //printf("mat2[%d][%d] = %d \n",k,j,mat1[k][j]);
                //printf("mat1[%d][%d] = %d \n",i+stride_pos_H,k + stride_pos_W,mat1[i+stride_pos_H][k + stride_pos_W ]);
                //printf("sum = %d \n",sum);
            }
        }
    }
    return sum;
}
void Print_matrix(int **mat1,int input_M ,int input_N)
{
    for(int i=0; i < input_M; i++)
    {
	    for(int j=0; j< input_N; j++)
	    {
            printf("%d\t",mat1[i][j]);
        }
        printf("\n");
    }

}

void conv2d_impl(int **input, int **filter, int **conv2d_res , 
                    int input_H, int input_W, int filter_H, int filter_W, int output_H, int output_W)
{
     for (int i=0;i<output_H;i++)
     {
         for (int j=0; j<output_W;j++)
         {
             conv2d_res[i][j] = conv_matrix_multiply(input, filter, input_H,  input_W, 
                                                        filter_H, filter_W,
                                                         i,j);
             //break;
         }
         //break;
     }
}

int maxpool2d_impl(int **conv2d_res, int pool_H,  int pool_W,int pool_pos_H,int pool_pos_W)
{
    //printf("------------------- maxpool_2d --------------------\n");
    int large_value = conv2d_res[pool_pos_H][pool_pos_W];
    for (int i = 0; i < pool_H; i++) {
        for (int j = 0; j < pool_H; j++) {
            int curr_value = conv2d_res[i+pool_pos_H][j + pool_pos_W ];
            if(large_value < curr_value);
            {
                large_value  = curr_value;
            }
        }
    }
    return large_value;
}

void pool2d_impl(int **conv2d_res ,int **pool_res,
                    int pool_H, int pool_W,int pool_output_H,int pool_output_W)
{
     for (int i=0;i<pool_output_H;i++)
     {
         for (int j=0; j<pool_output_W;j++)
         {
             pool_res[i][j] = maxpool2d_impl(conv2d_res, pool_H,  pool_W,i,j);
             //break;
         }
         //break;
     }


}

int main()
{

    int **input,**filter,**res;

    int **conv2d_res,**pool_res,*flatten_res,*softmax_res;
    int input_H=5 , input_W =5;
    int filter_H = 3,filter_W = 3,padding= 0 ,strides = 1;
    int pool_H = 2, pool_W =2,pool_output_H =0,pool_output_W =0,pool_strides =1;

    int conv_output_H = 0,conv_output_W =0;
    
    /*
        Output height = (Input height + padding height top + padding height bottom - kernel height) / (stride height) + 1

        Output width = (Output width + padding width right + padding width left - kernel width) / (stride width) + 1
    */
    conv_output_H = (input_H - filter_H)/strides + 1;
    conv_output_W = (input_W - filter_W)/strides + 1;

    //printf("conv_output_H = %d  \n",conv_output_H);
    //printf("conv_output_W = %d  \n",conv_output_W);

    input = (int **)malloc(input_H * sizeof *input);
    for (int i=0; i<input_H; i++)
    {
        input[i] = (int *)malloc(input_W * sizeof *input[i]);
    }

    filter = (int **)malloc(filter_H * sizeof *filter);
    for (int i=0; i<filter_H; i++)
    {
        filter[i] = (int *)malloc(filter_W * sizeof *filter[i]);
    }

    res = (int **)malloc(input_H * sizeof *res);
    for (int i=0; i<input_H; i++)
    {
        res[i] = (int *)malloc(input_W * sizeof *input[i]);
    }
    
    conv2d_res = (int **)malloc(input_H * sizeof *conv2d_res);
    for (int i=0; i<conv_output_H; i++)
    {
        conv2d_res[i] = (int *)malloc(conv_output_W * sizeof *conv2d_res[i]);
    }
    

    for(int i=0; i < input_H; i++)
    {
    	for(int j=0; j< input_W; j++)
    	{
            input[i][j] = i*input_W + j + 1;
        }
    }
    
    for(int i=0; i < filter_H; i++)
    {
	    for(int j=0; j< filter_W; j++)
	    {
            filter[i][j] = i*filter_W + j + 1;
        }
    }
    printf("----------- Input ----------------\n\n"); 
    Print_matrix(input, input_H, input_W);
    printf("----------- Filter ----------------\n\n");
    Print_matrix(filter, filter_H, filter_W);
    //matrix_multiply(input, filter, res, input_H, input_W, filter_H, filter_W);
    conv2d_impl(input, filter,conv2d_res , input_H, input_W, filter_H, filter_W, conv_output_H, conv_output_W);
    pool_output_H = (conv_output_H - pool_H)/pool_strides + 1;
    pool_output_W = (conv_output_W - pool_W)/pool_strides + 1;
    printf("\n\nconv_output_H = %d  \n",conv_output_H);
    printf("conv_output_W = %d  \n\n",conv_output_W);
    printf("----------- conv2d_result ----------------\n\n");
    Print_matrix(conv2d_res,conv_output_H,conv_output_W);
    pool_res = (int **)malloc(pool_output_H * sizeof *pool_res);
    for (int i=0; i<pool_output_H; i++)
    {
        pool_res[i] = (int *)malloc(pool_output_W * sizeof *pool_res[i]);
    }
    pool2d_impl(conv2d_res,pool_res,pool_H,pool_W,pool_output_H,pool_output_W);
    printf("----------- Max_pool2d_result ----------------\n\n");
    Print_matrix(pool_res,pool_output_H,pool_output_W);
}