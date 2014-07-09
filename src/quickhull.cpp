#include "quickhull.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <valarray>
#include <chrono>

#include <cstdlib>
#include <cstdio>

#include <iostream>

#include <cstdlib>
#include <cassert>

int
main(int argc, char * argv[])
{
    using size_type = std::size_t;

    std::ifstream ifs_;
    if (!(argc < 2)) {
        ifs_.open(argv[1]); // rbox D3 s 100 > points.txt
        if (!ifs_.is_open()) {
            std::cerr << "file is not open" << std::endl;
            return EXIT_FAILURE;
        }
    }
    std::istream & in_ = (argc < 2) ? std::cin : ifs_;
    std::cout << "read file: " << ((argc < 2) ? "stdin" : argv[1]) << std::endl;
    std::string line_;
    if (!std::getline(in_, line_)) {
        std::cerr << "bad file format" << std::endl;
        return EXIT_FAILURE;
    }
    size_type dim_;
    std::istringstream iss(line_);
    iss >> dim_;
    {
        using char_type = typename std::string::value_type;
        std::cout << "command line:";
        std::istreambuf_iterator< char_type > const ibeg(iss), iend;
        std::copy(ibeg, iend, std::ostreambuf_iterator< char_type >(std::cout));
        std::cout << std::endl;
    }
    if (!std::getline(in_, line_)) {
        std::cerr << "no count at second line" << std::endl;
        return EXIT_FAILURE;
    }
    using G = float;
    using point_type = std::valarray< G >;
    using points_type = std::valarray< point_type >;
    size_type const count_ = std::stoll(line_);
    points_type points_(count_);
    for (size_type i = 0; i < count_; ++i) {
        if (!std::getline(in_, line_)) {
            std::cerr << "io: line count error" << std::endl;
            return EXIT_FAILURE;
        }
        point_type & point_ = points_[i];
        point_.resize(dim_);
        iss.str(line_);
        std::copy_n(std::istream_iterator< G >(iss), dim_, std::begin(point_));
        if (!iss) {
            std::cerr << "bad value at line " << points_.size() << " of data" << std::endl;
            return EXIT_FAILURE;
        }
    }
    std::cout.rdbuf()->pubsetbuf(nullptr, 0);
    std::cout << "D = " << dim_ << std::endl;
    std::cout << "N = " << count_ << std::endl;
    using quick_hull_type = quick_hull< points_type >;
    quick_hull_type quick_hull_(dim_, points_);
    {
        using std::chrono::duration_cast;
        using std::chrono::microseconds;
        using std::chrono::steady_clock;
        {
            steady_clock::time_point const start = steady_clock::now();
            size_type const basis_size_ = quick_hull_.create_simplex().size();
            steady_clock::time_point const end = steady_clock::now();
            std::cout << "simplex time = " << duration_cast< microseconds >(end - start).count() << "us" << std::endl;
            if (basis_size_ != dim_ + 1) {
                std::cerr << "cannot create a simplex" << std::endl;
                return EXIT_FAILURE;
            }
        }
        {
            steady_clock::time_point const start = steady_clock::now();
            quick_hull_.create_convex_hull();
            steady_clock::time_point const end = steady_clock::now();
            std::cout << "qh time = " << duration_cast< microseconds >(end - start).count() << "us" << std::endl;
        }
    }
    auto const & facets_ = quick_hull_.facets_;
    size_type const facets_count_ = facets_.size();
    std::cout << "number of facets: " << facets_count_ << std::endl;
#if 1
    std::ofstream ofs_;
    ofs_.open("script.txt"); // gnuplot> load 'script.txt'
    if (!ofs_.is_open()) {
        std::cerr << "output file cannot be truncated" << std::endl;
        return EXIT_FAILURE;
    }
    ofs_ << "clear" << std::endl;
    ofs_ << "set autoscale" << std::endl;
    switch (dim_) {
    case 1 : {
        ofs_ << "plot";
        break;
    }
    case 2 : {
        ofs_ << "plot";
        break;
    }
    case 3 : {
        ofs_ << "splot";
        break;
    }
    default : {
        std::cerr << "dimensionality value (" << dim_ << ") is out of supported range" << std::endl;
        return EXIT_FAILURE;
    }
    }
    ofs_ << " '-' with points notitle, '-' with labels offset character 0,character 1 notitle";
    for (std::size_t i = 0; i < facets_count_; ++i) {
        ofs_ << ", '-' with lines notitle";
    }
    ofs_ << ';' << std::endl;
    for (std::size_t i = 0; i < count_; ++i) {
        point_type const & point_ = points_[i];
        for (G const & coordinate_ : point_) {
            ofs_ << coordinate_ << ' ';
        }
        ofs_ << std::endl;
    }
    ofs_ << 'e' << std::endl;
    for (std::size_t i = 0; i < count_; ++i) {
        point_type const & point_ = points_[i];
        for (G const & coordinate_ : point_) {
            ofs_ << coordinate_ << ' ';
        }
        ofs_ << i << std::endl;
    }
    ofs_ << 'e' << std::endl;
    for (size_type i = 0; i < facets_count_; ++i) {
        auto const & vertices_ = facets_[i].vertices_;
        for (size_type const vertex_ : vertices_) {
            for (G const & coordinate_ : points_[vertex_]) {
                ofs_ << coordinate_ << ' ';
            }
            ofs_ << std::endl;
        }
        point_type const & first_vertex_ = points_[vertices_.front()];
        for (G const & coordinate_ : first_vertex_) {
            ofs_ << coordinate_ << ' ';
        }
        ofs_ << std::endl;
        ofs_ << 'e' << std::endl;
    }
#endif
    return EXIT_SUCCESS;
}
