// scilib.h
//

#ifndef SCILIB_H
#define SCILIB_H

#include <vector>
#include <complex>
#include <map>
#include <fstream>
#include <algorithm>
#include <random>
#include "snip.h"

AtomPtr fn_mean(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    if (list->tail.empty()) return make_atom(0.0);
    Real sum = 0;
    for (auto& e : list->tail) {
        sum += type_check(e, NUMBER)->value;
    }
    return make_atom(sum / list->tail.size());
}
AtomPtr fn_variance(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    if (list->tail.size() <= 1) return make_atom(0.0);
    Real m = type_check(fn_mean(node, env), NUMBER)->value;
    Real var = 0;
    for (auto& e : list->tail) {
        Real diff = type_check(e, NUMBER)->value - m;
        var += diff * diff;
    }
    return make_atom(var / (list->tail.size()));
}
AtomPtr fn_stddev(AtomPtr node, AtomPtr env) {
    AtomPtr var = fn_variance(node, env);
    return make_atom(std::sqrt(type_check(var, NUMBER)->value));
}
AtomPtr fn_distance(AtomPtr node, AtomPtr env) {
    AtomPtr a = type_check(node->tail.at(0), LIST);
    AtomPtr b = type_check(node->tail.at(1), LIST);
    if (a->tail.size() != b->tail.size()) error("vectors must have same size", node);
    Real sum = 0;
    for (size_t i = 0; i < a->tail.size(); ++i) {
        Real diff = type_check(a->tail[i], NUMBER)->value - type_check(b->tail[i], NUMBER)->value;
        sum += diff * diff;
    }
    return make_atom(std::sqrt(sum));
}
AtomPtr fn_linear_regression(AtomPtr node, AtomPtr env) {
    AtomPtr x_list = type_check(node->tail.at(0), LIST);
    AtomPtr y_list = type_check(node->tail.at(1), LIST);
    if (x_list->tail.size() != y_list->tail.size())
        error("linear-regression: x and y must have same length", node);
    size_t n = x_list->tail.size();
    bool is_1d = x_list->tail.size() > 0 && x_list->tail.at(0)->type == NUMBER;
    size_t dim = is_1d ? 1 : type_check(x_list->tail.at(0), LIST)->tail.size();
    size_t dim_b = dim + 1; // bias term
    // build XtX matrix (dim_b × dim_b)
    std::vector<std::vector<Real>> XtX(dim_b, std::vector<Real>(dim_b, 0.0));
    // build XtY vector (dim_b)
    std::vector<Real> XtY(dim_b, 0.0);
    for (size_t i = 0; i < n; ++i) {
        std::vector<Real> xi(dim_b, 1.0); // bias first
        if (is_1d) {
            xi[1] = type_check(x_list->tail.at(i), NUMBER)->value;
        } else {
            AtomPtr row = type_check(x_list->tail.at(i), LIST);
            for (size_t j = 0; j < dim; ++j) {
                xi[j+1] = type_check(row->tail.at(j), NUMBER)->value;
            }
        }
        Real yi = type_check(y_list->tail.at(i), NUMBER)->value;
        for (size_t j = 0; j < dim_b; ++j) {
            for (size_t k = 0; k < dim_b; ++k) {
                XtX[j][k] += xi[j] * xi[k];
            }
            XtY[j] += xi[j] * yi;
        }
    }
    // solve (XtX) w = XtY
    std::vector<std::vector<Real>> inv(dim_b, std::vector<Real>(dim_b, 0.0));
    if (dim_b == 2) {
        Real det = XtX[0][0]*XtX[1][1] - XtX[0][1]*XtX[1][0];
        if (std::abs(det) < 1e-8) error("singular matrix in regression", node);
        inv[0][0] =  XtX[1][1] / det;
        inv[0][1] = -XtX[0][1] / det;
        inv[1][0] = -XtX[1][0] / det;
        inv[1][1] =  XtX[0][0] / det;
    } else if (dim_b == 3) {
        Real a = XtX[0][0], b = XtX[0][1], c = XtX[0][2];
        Real d = XtX[1][0], e = XtX[1][1], f = XtX[1][2];
        Real g = XtX[2][0], h = XtX[2][1], i = XtX[2][2];
        Real det = a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
        if (std::abs(det) < 1e-8) error("singular matrix in regression", node);
        inv[0][0] = (e*i - f*h) / det;
        inv[0][1] = (c*h - b*i) / det;
        inv[0][2] = (b*f - c*e) / det;
        inv[1][0] = (f*g - d*i) / det;
        inv[1][1] = (a*i - c*g) / det;
        inv[1][2] = (c*d - a*f) / det;
        inv[2][0] = (d*h - e*g) / det;
        inv[2][1] = (b*g - a*h) / det;
        inv[2][2] = (a*e - b*d) / det;
    } else {
        error("linear regression currently supports up to 2 features", node);
    }
    // w = inv(XtX) * XtY
    std::vector<Real> w(dim_b, 0.0);
    for (size_t j = 0; j < dim_b; ++j) {
        for (size_t k = 0; k < dim_b; ++k) {
            w[j] += inv[j][k] * XtY[k];
        }
    }
    // --- corrected model output ---
    AtomPtr model = make_atom();
    for (size_t i = 1; i < w.size(); ++i) {
        model->tail.push_back(make_atom(w[i])); // slopes first
    }
    model->tail.push_back(make_atom(w[0])); // intercept last
    return model;
}
AtomPtr fn_predict_linear(AtomPtr node, AtomPtr env) {
    AtomPtr model = type_check(node->tail.at(0), LIST);
    AtomPtr x = node->tail.at(1);
    size_t n_features = model->tail.size() - 1; // last element is intercept
    Real intercept = type_check(model->tail.at(n_features), NUMBER)->value; // intercept at last position
    Real y = intercept;
    if (x->type == NUMBER) {
        if (n_features != 1)
            error("model dimension mismatch for scalar input", node);
        y += type_check(model->tail.at(0), NUMBER)->value * type_check(x, NUMBER)->value;
    } else if (x->type == LIST) {
        if (x->tail.size() != n_features)
            error("model dimension mismatch for list input", node);
        for (size_t i = 0; i < n_features; ++i) {
            y += type_check(model->tail.at(i), NUMBER)->value * type_check(x->tail.at(i), NUMBER)->value;
        }
    } else {
        error("input must be number or list", node);
    }
    return make_atom(y);
}
AtomPtr fn_kmeans(AtomPtr node, AtomPtr env) {
    AtomPtr points = type_check(node->tail.at(0), LIST);
    AtomPtr k_atom = type_check(node->tail.at(1), NUMBER);
    int k = static_cast<int>(k_atom->value);
    if (k <= 0) error("k must be > 0", node);

    std::vector<Real> centers;
    for (unsigned i = 0; i < (unsigned) k && i < points->tail.size(); ++i) {
        centers.push_back(type_check(points->tail[i], NUMBER)->value);
    }

    bool changed = true;
    for (int iter = 0; iter < 10 && changed; ++iter) {
        std::vector<std::vector<Real>> clusters(k);
        for (auto& p : points->tail) {
            Real val = type_check(p, NUMBER)->value;
            int best = 0;
            Real best_dist = std::abs(val - centers[0]);
            for (int j = 1; j < k; ++j) {
                Real dist = std::abs(val - centers[j]);
                if (dist < best_dist) {
                    best = j;
                    best_dist = dist;
                }
            }
            clusters[best].push_back(val);
        }

        changed = false;
        for (int j = 0; j < k; ++j) {
            if (clusters[j].empty()) continue;
            Real sum = 0;
            for (Real v : clusters[j]) sum += v;
            Real new_center = sum / clusters[j].size();
            if (std::abs(new_center - centers[j]) > 1e-6) {
                centers[j] = new_center;
                changed = true;
            }
        }
    }

    AtomPtr result = make_atom();
    for (Real c : centers) result->tail.push_back(make_atom(c));
    return result;
}
AtomPtr fn_knn(AtomPtr node, AtomPtr env) {
    AtomPtr train_x = type_check(node->tail.at(0), LIST);
    AtomPtr train_y = type_check(node->tail.at(1), LIST);
    AtomPtr query = type_check(node->tail.at(2), LIST);
    AtomPtr k_atom = type_check(node->tail.at(3), NUMBER);
    int k = static_cast<int>(k_atom->value);
    if (train_x->tail.size() != train_y->tail.size()) error("train_x and train_y must match", node);
    std::vector<std::pair<Real, AtomPtr>> dists;
    for (size_t i = 0; i < train_x->tail.size(); ++i) {
        AtomPtr p = type_check(train_x->tail[i], LIST);
        if (p->tail.size() != query->tail.size()) error("dimension mismatch", node);
        Real dist = 0;
        for (size_t j = 0; j < p->tail.size(); ++j) {
            Real diff = type_check(p->tail[j], NUMBER)->value - type_check(query->tail[j], NUMBER)->value;
            dist += diff * diff;
        }
        dists.push_back({std::sqrt(dist), train_y->tail[i]});
    }
    std::sort(dists.begin(), dists.end(), [](auto& a, auto& b) { return a.first < b.first; });
    std::map<Real, int> votes;
    for (unsigned i = 0; i < (unsigned) k && i < dists.size(); ++i) {
        Real label = type_check(dists[i].second, NUMBER)->value;
        votes[label]++;
    }
    Real best_label = 0;
    int best_count = -1;
    for (auto& kv : votes) {
        if (kv.second > best_count) {
            best_label = kv.first;
            best_count = kv.second;
        }
    }
    return make_atom(best_label);
}
AtomPtr random_matrix(int rows, int cols) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<Real> dis(-1.0, 1.0);
    AtomPtr mat = make_atom();
    for (int i = 0; i < rows; ++i) {
        AtomPtr row = make_atom();
        for (int j = 0; j < cols; ++j) {
            row->tail.push_back(make_atom(dis(gen)));
        }
        mat->tail.push_back(row);
    }
    return mat;
}
AtomPtr zero_vector(int size) {
    AtomPtr vec = make_atom();
    for (int i = 0; i < size; ++i) {
        vec->tail.push_back(make_atom(0.0));
    }
    return vec;
}
AtomPtr fn_nn_init(AtomPtr node, AtomPtr env) {
    AtomPtr sizes = type_check(node->tail.at(0), LIST);
    AtomPtr activations = type_check(node->tail.at(1), LIST);
    if (sizes->tail.size() != activations->tail.size() + 1) {
        error("nn-init: activations must be one less than sizes", node);
    }
    
    AtomPtr net = make_atom();
    for (size_t i = 0; i < activations->tail.size(); ++i) {
        int in_size = type_check(sizes->tail.at(i), NUMBER)->value;
        int out_size = type_check(sizes->tail.at(i + 1), NUMBER)->value;
        AtomPtr layer = make_atom();
        layer->tail.push_back(random_matrix(out_size, in_size)); // weights
        layer->tail.push_back(zero_vector(out_size)); // biases
        layer->tail.push_back(type_check(activations->tail.at(i), STRING)); // activation
        net->tail.push_back(layer);
    }
    return net;
}
Real relu(Real x) { return x > 0 ? x : 0; }
Real relu_deriv(Real x) { return x > 0 ? 1 : 0; }
Real sigmoid(Real x) { return 1.0 / (1.0 + std::exp(-x)); }
Real sigmoid_deriv(Real x) { Real s = sigmoid(x); return s * (1 - s); }
std::vector<Real> matvec_mul(AtomPtr mat, const std::vector<Real>& vec) {
    std::vector<Real> result;
    for (auto& row : mat->tail) {
        Real sum = 0.0;
        for (size_t j = 0; j < row->tail.size(); ++j) {
            sum += type_check(row->tail.at(j), NUMBER)->value * vec.at(j);
        }
        result.push_back(sum);
    }
    return result;
}
void add_bias(std::vector<Real>& v, AtomPtr bias) {
    for (size_t i = 0; i < v.size(); ++i) {
        v[i] += type_check(bias->tail.at(i), NUMBER)->value;
    }
}
AtomPtr fn_nn_predict(AtomPtr node, AtomPtr env) {
    AtomPtr net = type_check(node->tail.at(0), LIST);
    AtomPtr input = type_check(node->tail.at(1), LIST);
    std::vector<Real> vec;
    for (auto& e : input->tail) {
        vec.push_back(type_check(e, NUMBER)->value);
    }
    for (auto& layer : net->tail) {
        AtomPtr weights = layer->tail.at(0);
        AtomPtr biases = layer->tail.at(1);
        AtomPtr activation = type_check(layer->tail.at(2), STRING);
        std::string act = activation->lexeme;
        vec = matvec_mul(weights, vec);
        add_bias(vec, biases);
        if (act == "relu") {
            for (auto& x : vec) x = relu(x);
        } else if (act == "sigmoid") {
            for (auto& x : vec) x = sigmoid(x);
        } else {
            error("unknown activation function in nn-predict", layer);
        }
    }
    AtomPtr out = make_atom();
    for (auto v : vec) {
        out->tail.push_back(make_atom(v));
    }
    return out;
}
void apply_activation_deriv(std::vector<Real>& vec, const std::vector<Real>& pre_act, const std::string& act) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (act == "relu") {
            vec[i] *= relu_deriv(pre_act[i]);
        } else if (act == "sigmoid") {
            vec[i] *= sigmoid_deriv(pre_act[i]);
        }
    }
}
AtomPtr fn_nn_train(AtomPtr node, AtomPtr env) {
    AtomPtr net = type_check(node->tail.at(0), LIST);
    AtomPtr input = type_check(node->tail.at(1), LIST);
    AtomPtr target = type_check(node->tail.at(2), LIST);
    Real lr = type_check(node->tail.at(3), NUMBER)->value;
    // Convert input and target to std::vector<Real>
    std::vector<Real> x;
    for (auto& e : input->tail) x.push_back(type_check(e, NUMBER)->value);
    std::vector<Real> y;
    for (auto& e : target->tail) y.push_back(type_check(e, NUMBER)->value);
    // Forward pass: store all activations and pre-activations
    std::vector<std::vector<Real>> activations = {x};
    std::vector<std::vector<Real>> pre_activations;
    for (auto& layer : net->tail) {
        AtomPtr weights = layer->tail.at(0);
        AtomPtr biases = layer->tail.at(1);
        std::string act = type_check(layer->tail.at(2), STRING)->lexeme;
        std::vector<Real> z = matvec_mul(weights, activations.back());
        add_bias(z, biases);
        pre_activations.push_back(z);
        std::vector<Real> a = z;
        if (act == "relu") {
            for (auto& v : a) v = relu(v);
        } else if (act == "sigmoid") {
            for (auto& v : a) v = sigmoid(v);
        } else {
            error("unknown activation", layer);
        }
        activations.push_back(a);
    }
    // Backward pass: compute gradients
    std::vector<Real> delta(activations.back().size());
    for (size_t i = 0; i < delta.size(); ++i) {
        delta[i] = activations.back()[i] - y[i];
    }
    apply_activation_deriv(delta, pre_activations.back(), type_check(net->tail.back()->tail.at(2), STRING)->lexeme);
    for (int l = net->tail.size() - 1; l >= 0; --l) {
        AtomPtr layer = net->tail.at(l);
        AtomPtr weights = layer->tail.at(0);
        AtomPtr biases = layer->tail.at(1);
        // Update weights
        for (size_t i = 0; i < weights->tail.size(); ++i) {
            for (size_t j = 0; j < weights->tail.at(i)->tail.size(); ++j) {
                Real old_w = type_check(weights->tail.at(i)->tail.at(j), NUMBER)->value;
                Real grad = delta[i] * activations[l][j];
                weights->tail.at(i)->tail.at(j) = make_atom(old_w - lr * grad);
            }
        }
        // Update biases
        for (size_t i = 0; i < biases->tail.size(); ++i) {
            Real old_b = type_check(biases->tail.at(i), NUMBER)->value;
            biases->tail.at(i) = make_atom(old_b - lr * delta[i]);
        }
        if (l > 0) {
            // Compute delta for previous layer
            std::vector<Real> new_delta(activations[l].size(), 0.0);
            for (size_t j = 0; j < activations[l].size(); ++j) {
                for (size_t i = 0; i < weights->tail.size(); ++i) {
                    new_delta[j] += type_check(weights->tail.at(i)->tail.at(j), NUMBER)->value * delta[i];
                }
            }
            apply_activation_deriv(new_delta, pre_activations[l-1], type_check(net->tail.at(l-1)->tail.at(2), STRING)->lexeme);
            delta = new_delta;
        }
    }
    return make_atom(""); // no output needed
}
using Complex = std::complex<Real>;
void fft_compute(std::vector<Complex>& a, bool invert) {
    size_t n = a.size();
    if (n == 1) return;
    std::vector<Complex> a0(n / 2), a1(n / 2);
    for (size_t i = 0; 2 * i < n; ++i) {
        a0[i] = a[i*2];
        a1[i] = a[i*2+1];
    }
    fft_compute(a0, invert);
    fft_compute(a1, invert);
    Real ang = 2 * M_PI / n * (invert ? -1 : 1);
    Complex w(1), wn(cos(ang), sin(ang));
    for (size_t i = 0; 2 * i < n; ++i) {
        a[i] = a0[i] + w * a1[i];
        a[i + n/2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2;
            a[i + n/2] /= 2;
        }
        w *= wn;
    }
}
bool is_power_of_two(size_t n) {
    return (n > 0) && ((n & (n-1)) == 0);
}
AtomPtr fn_fft(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    std::vector<Complex> data;
    for (auto& elem : list->tail) {
        data.push_back(type_check(elem, NUMBER)->value);
    }
    if (!is_power_of_two(data.size())) {
        error("input size must be a power of two", node);
    }
    fft_compute(data, false);
    AtomPtr out = make_atom();
    for (auto& c : data) {
        AtomPtr pair = make_atom();
        pair->tail.push_back(make_atom(c.real()));
        pair->tail.push_back(make_atom(c.imag()));
        out->tail.push_back(pair);
    }
    return out;
}
AtomPtr fn_ifft(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    std::vector<Complex> data;
    for (auto& elem : list->tail) {
        AtomPtr p = type_check(elem, LIST);
        Real re = type_check(p->tail.at(0), NUMBER)->value;
        Real im = type_check(p->tail.at(1), NUMBER)->value;
        data.push_back(Complex(re, im));
    }
    if (!is_power_of_two(data.size())) {
        error("input size must be a power of two", node);
    }	
    fft_compute(data, true);
    AtomPtr out = make_atom();
    for (auto& c : data) {
        out->tail.push_back(make_atom(c.real()));
    }
    return out;
}
AtomPtr fn_pol2car(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    AtomPtr out = make_atom();
    for (auto& elem : list->tail) {
        AtomPtr pair = type_check(elem, LIST);
        Real r = type_check(pair->tail.at(0), NUMBER)->value;
        Real theta = type_check(pair->tail.at(1), NUMBER)->value;
        AtomPtr cartesian = make_atom();
        cartesian->tail.push_back(make_atom(r * std::cos(theta)));
        cartesian->tail.push_back(make_atom(r * std::sin(theta)));
        out->tail.push_back(cartesian);
    }
    return out;
}
AtomPtr fn_car2pol(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    AtomPtr out = make_atom();
    for (auto& elem : list->tail) {
        AtomPtr pair = type_check(elem, LIST);
        Real x = type_check(pair->tail.at(0), NUMBER)->value;
        Real y = type_check(pair->tail.at(1), NUMBER)->value;
        AtomPtr polar = make_atom();
        polar->tail.push_back(make_atom(std::sqrt(x*x + y*y)));
        polar->tail.push_back(make_atom(std::atan2(y, x)));
        out->tail.push_back(polar);
    }
    return out;
}
AtomPtr fn_conv(AtomPtr node, AtomPtr env) {
    AtomPtr a = type_check(node->tail.at(0), LIST);
    AtomPtr b = type_check(node->tail.at(1), LIST);
    std::vector<Complex> A, B;
    for (auto& elem : a->tail) A.push_back(type_check(elem, NUMBER)->value);
    for (auto& elem : b->tail) B.push_back(type_check(elem, NUMBER)->value);
    size_t n = 1;
    while (n < A.size() + B.size()) n <<= 1;
    A.resize(n, 0.0);
    B.resize(n, 0.0);
    fft_compute(A, false);
    fft_compute(B, false);
    for (size_t i = 0; i < n; ++i) A[i] *= B[i];
    fft_compute(A, true);
    AtomPtr out = make_atom();
    for (auto& c : A) {
        out->tail.push_back(make_atom(c.real()));
    }
    return out;
}
AtomPtr fn_dot(AtomPtr node, AtomPtr env) {
    AtomPtr a = type_check(node->tail.at(0), LIST);
    AtomPtr b = type_check(node->tail.at(1), LIST);
    const auto& atail = a->tail;
    const auto& btail = b->tail;
    size_t n = atail.size();
    if (n != btail.size()) {
        error("mismatched vector lengths", node);
    }
    Real sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    size_t i = 0;
    // process blocks of 4
    for (; i + 3 < n; i += 4) {
        sum0 += type_check(atail[i+0], NUMBER)->value * type_check(btail[i+0], NUMBER)->value;
        sum1 += type_check(atail[i+1], NUMBER)->value * type_check(btail[i+1], NUMBER)->value;
        sum2 += type_check(atail[i+2], NUMBER)->value * type_check(btail[i+2], NUMBER)->value;
        sum3 += type_check(atail[i+3], NUMBER)->value * type_check(btail[i+3], NUMBER)->value;
    }
    for (; i < n; ++i) {
        sum0 += type_check(atail[i], NUMBER)->value * type_check(btail[i], NUMBER)->value;
    }

    return make_atom(sum0 + sum1 + sum2 + sum3);
}
AtomPtr fn_writewav(AtomPtr node, AtomPtr env) {
    std::string filename = type_check(node->tail.at(0), STRING)->lexeme;
    AtomPtr data = type_check(node->tail.at(1), LIST);
    uint16_t bits = 16;
    uint32_t samplerate = 44100;
    if (node->tail.size() >= 3) {
        bits = static_cast<uint16_t>(type_check(node->tail.at(2), NUMBER)->value);
        if (bits != 16 && bits != 32) error("bits must be 16 or 32", node);
    }
    if (node->tail.size() >= 4) {
        samplerate = static_cast<uint32_t>(type_check(node->tail.at(3), NUMBER)->value);
    }
    uint16_t channels = data->tail.size();
    if (channels == 0) error("empty channel list", node);
    uint32_t samples = data->tail.at(0)->tail.size();
    for (auto& chan : data->tail) {
        if (chan->tail.size() != samples)
            error("all channels must have same length", node);
    }
    uint32_t bytes_per_sample = bits / 8;
    uint32_t data_size = samples * channels * bytes_per_sample;
    uint32_t file_size = 36 + data_size;
    std::ofstream file(filename, std::ios::binary);
    if (!file) error("cannot create WAV file", node);
    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&file_size), 4);
    file.write("WAVEfmt ", 8);
    uint32_t fmt_size = 16;
    uint16_t audio_fmt = 1;
    file.write(reinterpret_cast<const char*>(&fmt_size), 4);
    file.write(reinterpret_cast<const char*>(&audio_fmt), 2);
    file.write(reinterpret_cast<const char*>(&channels), 2);
    file.write(reinterpret_cast<const char*>(&samplerate), 4);
    uint32_t byterate = samplerate * channels * bits / 8;
    file.write(reinterpret_cast<const char*>(&byterate), 4);
    uint16_t blockalign = channels * bits / 8;
    file.write(reinterpret_cast<const char*>(&blockalign), 2);
    file.write(reinterpret_cast<const char*>(&bits), 2);
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&data_size), 4);
    for (size_t i = 0; i < samples; ++i) {
        for (size_t ch = 0; ch < channels; ++ch) {
            Real v = type_check(data->tail.at(ch)->tail.at(i), NUMBER)->value;
            if (v > 1.0) v = 1.0;
            if (v < -1.0) v = -1.0;
            if (bits == 16) {
                int16_t sample = static_cast<int16_t>(v * 32767.0);
                file.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
            } else if (bits == 32) {
                int32_t sample = static_cast<int32_t>(v * 2147483647.0);
                file.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
            }
        }
    }
    return make_atom();
}
AtomPtr fn_readwav(AtomPtr node, AtomPtr env) {
    std::string filename = type_check(node->tail.at(0), STRING)->lexeme;
    std::ifstream file(filename, std::ios::binary);
    if (!file) error("cannot open WAV file", node);
    char header[44];
    file.read(header, 44);
    if (std::string(header, header+4) != "RIFF" || std::string(header+8, header+12) != "WAVE")
        error("invalid WAV header", node);
    uint16_t channels = *reinterpret_cast<uint16_t*>(header + 22);
    // uint32_t samplerate = *reinterpret_cast<uint32_t*>(header + 24);
    uint16_t bits = *reinterpret_cast<uint16_t*>(header + 34);
    if (bits != 16 && bits != 32) error("only 16-bit or 32-bit PCM supported", node);
    uint32_t data_size = *reinterpret_cast<uint32_t*>(header + 40);
    size_t samples = data_size / (channels * (bits / 8));
    std::vector<std::vector<Real>> out(channels, std::vector<Real>(samples));
    for (size_t i = 0; i < samples; ++i) {
        for (size_t ch = 0; ch < channels; ++ch) {
            if (bits == 16) {
                int16_t sample;
                file.read(reinterpret_cast<char*>(&sample), sizeof(sample));
                out[ch][i] = sample / 32768.0;
            } else if (bits == 32) {
                int32_t sample;
                file.read(reinterpret_cast<char*>(&sample), sizeof(sample));
                out[ch][i] = sample / 2147483648.0;
            }
        }
    }
    AtomPtr result = make_atom();
    for (auto& chan : out) {
        AtomPtr c = make_atom();
        for (auto v : chan) c->tail.push_back(make_atom(v));
        result->tail.push_back(c);
    }
    return result;
}
AtomPtr fn_readcsv(AtomPtr node, AtomPtr env) {
    std::string filename = type_check(node->tail.at(0), STRING)->lexeme;
    std::ifstream file(filename);
    if (!file.is_open()) {
        error("cannot open file", node);
    }
    AtomPtr result = make_atom();
    std::string line;
    while (std::getline(file, line)) {
        AtomPtr row = make_atom();
        std::stringstream ss(line);
        std::string item;
        while (std::getline(ss, item, ',')) {
            if (is_number(item)) {
                row->tail.push_back(make_atom(std::stod(item)));
            } else {
                row->tail.push_back(make_atom('"' + item));
            }
        }
        result->tail.push_back(row);
    }
    return result;
}
AtomPtr fn_writecsv(AtomPtr node, AtomPtr env) {
    std::string filename = type_check(node->tail.at(0), STRING)->lexeme;
    AtomPtr table = type_check(node->tail.at(1), LIST);
    std::ofstream file(filename);
    if (!file.is_open()) {
        error("cannot open file", node);
    }
    for (const auto& row : table->tail) {
        AtomPtr r = type_check(row, LIST);
        for (size_t i = 0; i < r->tail.size(); ++i) {
            if (r->tail[i]->type == NUMBER) {
                file << r->tail[i]->value;
            } else if (r->tail[i]->type == STRING) {
                file << r->tail[i]->lexeme;
            } else if (r->tail[i]->type == SYMBOL) {
                file << r->tail[i]->lexeme;
            }
            if (i != r->tail.size() - 1) file << ",";
        }
        file << "\n";
    }
    return make_atom("");
}
void add_scientific (AtomPtr env) {
    add_op ("mean", &fn_mean, 1, env);
	add_op ("variance", &fn_variance, 1, env);
	add_op ("stddev", &fn_stddev, 1, env);
	add_op ("distance", &fn_distance, 2, env);
	add_op ("kmeans", &fn_kmeans, 2, env);
	add_op ("linreg", &fn_linear_regression, 2, env);
	add_op ("linreg-predict", &fn_predict_linear, 2, env);
	add_op ("knn", &fn_knn, 4, env);	
    add_op ("nn-init", &fn_nn_init, 2, env);
    add_op ("nn-predict", &fn_nn_predict, 2, env);	
    add_op ("nn-train", &fn_nn_train, 4, env);	
	add_op ("fft", &fn_fft, 1, env);
	add_op ("ifft", &fn_ifft, 1, env);
	add_op ("conv", &fn_conv, 2, env);
	add_op ("dot", &fn_dot, 2, env);
	add_op ("pol2car", &fn_pol2car, 1, env);
	add_op ("car2pol", &fn_car2pol, 1, env);	
	add_op ("readwav", &fn_readwav, 1, env);
	add_op ("writewav", &fn_writewav, 3, env);
	add_op ("readcsv", &fn_readcsv, 1, env);
	add_op ("writecsv", &fn_writecsv, 2, env);
}
#endif // SCILIB_h

// eof



