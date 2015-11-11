#include <vector>

extern "C" {
	#include <Python.h>
}

namespace graph_algorithm
{
	typedef std::vector<double>  row_t;
	typedef std::vector<row_t>   matrix_t;

	static matrix_t ololo(matrix_t a){
		for (size_t k = 0; k < a.size(); ++k){
			for (size_t i = 0; i < a.size(); ++i){
				for (size_t j = 0; j < a.size(); ++j){
					if (a[i][j] != 0 && (a[i][k] + a[k][j]) != 0){
						a[i][j] = 1 / (1 / a[i][j] + 1 / (a[i][k] + a[k][j]));
					}
				}
			}
		}
		return a;
	}
}

static PyObject * cxx_to_pyobject(const graph_algorithm::matrix_t &matrix)
{
	PyObject * result = PyList_New(matrix.size());
	for (size_t i = 0; i < matrix.size(); ++i) {
		const graph_algorithm::row_t & row = matrix[i];
		PyObject * py_row = PyList_New(row.size());
		PyList_SetItem(result, i, py_row);
		for (size_t j = 0; j < row.size(); ++j) {
			const double elem = row[j];
			PyObject * py_elem = PyFloat_FromDouble(elem);
			PyList_SetItem(py_row, j, py_elem);
		}
	}
	return result;
}

static graph_algorithm::matrix_t pyobject_to_cxx(PyObject * py_matrix)
{
	graph_algorithm::matrix_t result;
	result.resize(PyObject_Length(py_matrix));
	for (size_t i = 0; i < result.size(); ++i) {
		PyObject * py_row = PyList_GetItem(py_matrix, i);
		graph_algorithm::row_t & row = result[i];
		row.resize(PyObject_Length(py_row));
		for (size_t j = 0; j < row.size(); ++j) {
			PyObject * py_elem = PyList_GetItem(py_row, j);
			const double elem = PyFloat_AsDouble(py_elem);
			row[j] = elem;
		}
	}
	return result;
}

static PyObject* func(PyObject* mod, PyObject* args)
{
	PyObject* py_matrix = PyTuple_GetItem(args, 0);
	
	graph_algorithm::matrix_t matrix = pyobject_to_cxx(py_matrix);

	graph_algorithm::matrix_t result = graph_algorithm::ololo(matrix);

	PyObject* py_result = cxx_to_pyobject(result);
	return py_result;
} 

PyMODINIT_FUNC PyInit_graph_algorithm()
{
	static PyMethodDef ModMethods[] = {
		{ "FW", func, METH_VARARGS, "Floyd Warshall realization" },
		{ NULL, NULL, 0, NULL }
	};

	static PyModuleDef ModDef = {
		PyModuleDef_HEAD_INIT,
		"graph_algorithm",
		"My fucking module",
		-1, ModMethods,
		NULL, NULL, NULL, NULL
	};

	PyObject * module = PyModule_Create(&ModDef);
	return module;
}