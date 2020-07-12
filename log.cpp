#include <iostream>
#include <chrono>
#include <cmath>
#include <cstring>

#define PI 3.141516f

struct sImage {
    int width, height;
    int margin;
    float *data;

    sImage(const int i_margin) {
        margin = i_margin;
        data = NULL;
    };

    ~sImage() {
        if (data != NULL) {
            delete data;
        }
    };

    /**
     * (Main Function!)
     * Copies a secuence of 1-D raw image data, 
     *  and adds the margins border
    */
    void copy_into(const float* i_data, const int data_width, const int data_heigth) {
        width = data_width, height = data_heigth;

        // Data cleanup
        if (data != NULL) {
            delete data;
        }
        data = new float[(width + margin) * (height + margin)];
        memset(data, 0.f, sizeof(float) * (width + margin) * (height + margin));

        #pragma omp parallel for
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                data[(x * (width + margin)) + y] = i_data[(x * width) + y];
            }
        }

    };

    /**
     * Wrapper for cleaning the accesing the image, taking into
     * accout the margins
    */
    inline float get_with_margins(const int x, const int y) const {
        return data[(x * (width + margin)) + y];
    };

    /**
     * Wrapper for cleaning the accesing the image
    */
    inline float get(const int x, const int y) const {
        return data[(x * width) + y];
    };
};

struct sLoG {
    float *kernel;
    int kernel_size;
    float std_desv;

    sLoG(const int kernel_side_size, const float desv) {
        kernel_size = kernel_side_size;
        std_desv = desv;

        kernel = new float[kernel_size * kernel_size];
        generate_kernel();
    }

    ~sLoG() {
        delete kernel;
    }

    /**
     * Generate the Laplacian of Gaussian discrete kernel
     * for convolution, according to the formula:
     * 
     * See pic on readme
     * 
    */
    void generate_kernel() {
        // Compute parts of the equation that are not dependant
        // on the x, y factors
        float p1 = -1.f / (PI * pow(std_desv, 4.0f));
        float p2 = 2.f * pow(std_desv, 2.0f);

        for (int x = 0; x < kernel_size; x++) {
            float x_2 = pow(x, 2.0f);//x * x;

            for (int y = 0; y < kernel_size; y++) {
                float y_2 = pow(y, 2.0f);
                float p3 = - ((x_2 + y_2) / p2);
                float tmp = p1;

                tmp *= 1.f + p3;
                tmp *= exp(p3);

                kernel[(x * kernel_size) + y] = tmp;
            }
        }
    }

    /**
     * Wrapper for cleaning up the acces to the filter
    */
    inline float get(const int x, const int y) const {
        return kernel[(x * kernel_size) + y];
    };


    /**
     * (Main Function!)
     * Computes the average Laplacian of Gaussian score
     * for a given image, usigin the previusly calculated
     * convolutional filter
    */
    float compute_avg_LoG(const sImage *img) {
        float result = 0.0f;
        int img_width = img->width, img_heigth = img->height;
        int kernel_half = kernel_size / 2.0f;

        #pragma omp parallel for collapse(2) reduction(+:result)
        for (int i_x = 0; i_x < img_width; i_x++) {
            for (int i_y = 0; i_y < img_heigth; i_y++) {
                float tmp = 0.0f;

                // Convolutional operation optimized with SIMD
                #pragma omp simd collapse(2) reduction(+:tmp)
                for (int f_x = 0; f_x < kernel_size; f_x++) {
                    for (int f_y = 0; f_y < kernel_size; f_y++) {
                        int i_f_x = f_x + i_x - kernel_half; // Image's filter indexess
                        int i_f_y = f_y + i_y - kernel_half;

                        tmp += get(f_x, f_y) * img->get_with_margins(i_f_x, i_f_y);
                    }
                }

                result += tmp;
            }
        }

        return result;
    }
};

int main() {
    sLoG log_op = sLoG(10, 1.4f);
    sImage img = sImage(10);

    // Iterative test multiple batch sizes
    for (int j = 0; j < 3; j++) {
        int size = pow(100.0f, j);
        float res;

        // Prepare test data
        float* ex_data = new float[size * size];
        for (int i = 0; i < size * size; i++) {
            ex_data[i] = i;
        }

        float avg_result = 0.0f;
        for (int i = 0; i <= 100; i++) {
            // Calculate and measure time
            auto start_timer = std::chrono::steady_clock::now();
            img.copy_into(ex_data, 100, 100);
            res = log_op.compute_avg_LoG(&img);
            auto end_timer = std::chrono::steady_clock::now();

            avg_result = std::chrono::duration_cast<std::chrono::microseconds>(end_timer - start_timer).count();
        }
        
        avg_result /= 100;

        // My god, what is this monster
        std::cout << "LoG Size: " << size << " Result: " << res <<  " Time: "<< avg_result << std::endl;

        delete ex_data;
    }

    

    return 0;
};