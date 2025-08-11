#define _POSIX_C_SOURCE 200809L //used for getline since getline is in POSIX
#include "hw7.h"


bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) {
    if(root == NULL) { //inserting node to an empty bst
        bst_sf* node = malloc(sizeof(bst_sf));
        if (node == NULL) return NULL; //memory allocation failed 
        node->mat = mat;
        node->left_child = NULL;
        node->right_child = NULL;
        return node;
    }
    else{ //not empty bst, so we recursively find where to insert this node 
        char new_name = mat->name;
        char cur_name = root->mat->name;
        if (new_name < cur_name){ //go to left subtree if smaller
            root->left_child = insert_bst_sf(mat, root->left_child);
        }
        else{ //go to right subtree cuz bigger than current node
            root->right_child = insert_bst_sf(mat, root->right_child);
        }
    }
    return root;
}

matrix_sf* find_bst_sf(char name, bst_sf *root) {
    while (root != NULL){ //keep going until we find node or node don'm exist
        char cur = root->mat->name; 
        if (name == cur){ //found node
            return root->mat;
        }
        else if (name < cur){ //go to left subtree if smaller
            root = root->left_child;
        }
        else{ //go right subtree cuz bigger
            root = root->right_child;
        }
    }
    return NULL;
}

void free_bst_sf(bst_sf *root) {
    if (root == NULL) return; 

    free_bst_sf(root->left_child);
    free_bst_sf(root->right_child);

    free(root->mat);
    free(root);
}


matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    int rows = mat1->num_rows;
    int cols = mat1->num_cols;
    unsigned int elements = rows * cols; //total elements in matrix

    matrix_sf *m = malloc(sizeof(matrix_sf) + elements * sizeof(int)); 
    if (!m) return NULL; //check if allocation fails

    m->name = '?'; //set name to temp result 
    m->num_rows = rows;
    m->num_cols = cols;

    for (size_t i = 0; i < elements; i++){ //go through every element and add values from both matrix into new matrix
        m->values[i] = mat1->values[i] + mat2->values[i];
    }

    return m;
}

matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    unsigned int row1 = mat1->num_rows;
    unsigned int col1 = mat1->num_cols;
    unsigned int col2 = mat2->num_cols;

    unsigned int elements = row1 * col2; //total elements in matrix 

    matrix_sf *m = malloc(sizeof(matrix_sf) + elements * sizeof(int));
    if(m == NULL) return NULL;

    m->name = '?';
    m->num_rows = row1;
    m->num_cols = col2;

    for (unsigned int i = 0; i < row1; i++){
        for(unsigned int j = 0; j < col2; j++){
            int acc = 0; //accumulates sum of products of a cell
            for (unsigned int k = 0; k < col1; k++){
                size_t a_index = (size_t)i * col1+ k;
                size_t b_index = (size_t)k * col2 + j;
                acc += mat1->values[a_index] * mat2->values[b_index]; //accumulates sum product of that cell
            }
            m->values[(size_t)i*col2 + j] = acc; //write the result to cell in new matrix 
        }
    }
    return m;

}

matrix_sf* transpose_mat_sf(const matrix_sf *mat) {
    unsigned int rows = mat->num_rows;   
    unsigned int cols = mat->num_cols; 
    unsigned int elements = rows * cols;

    matrix_sf *m = malloc(sizeof(matrix_sf) + elements * sizeof(int));
    if (m == NULL) return NULL;

    m->name = '?';
    //switch the rows and cols
    m->num_rows = cols; 
    m->num_cols = rows;

    // out(j, i) = in(i, j)
    for (unsigned int i = 0; i < rows; i++) {
        // size_t in_row_base = (size_t)i * cols;
        for (unsigned int j = 0; j < cols; j++) {
            size_t in_index  = i * cols + j; //index of (i,j)       
            size_t out_index = j * rows + i; //index of (j,i)
            m->values[out_index] = mat->values[in_index]; //copy elements from (i,j) to (j,i)
        }
    }
    return m;
}

matrix_sf* create_matrix_sf(char name, const char *expr) {

    const char *curr_char = expr; //points to current character as we parse the string
    char *end = NULL; 

    //parsing number of rows
    while (*curr_char && isspace((unsigned char)*curr_char)){
        curr_char++;
    }
    unsigned long ul_rows = strtoul(curr_char, &end, 10);
    curr_char = end;

    //parsing number of columns
    while (*curr_char && isspace((unsigned char)*curr_char)){
        curr_char++;
    }
    unsigned long ul_cols = strtoul(curr_char, &end, 10);
    curr_char = end;

    unsigned int rows = (unsigned int)ul_rows;
    unsigned int cols = (unsigned int)ul_cols;

    //look for opening bracket and get the first number inside it 
    while (*curr_char && *curr_char != '['){
        curr_char++;
    }
    if (*curr_char == '['){
        curr_char++;
    }

    size_t elements = (size_t)rows * (size_t)cols; 
    
    //allocate memory for matrix
    matrix_sf *m = malloc(sizeof(matrix_sf) + elements * sizeof(int));
    if (m == NULL) return NULL;

    m->name = name;
    m->num_rows = rows;
    m->num_cols = cols;

    //fill in data for new matrix 
    for (unsigned int r = 0; r < rows; ++r) {
        for (unsigned int c = 0; c < cols; ++c) {
            while (*curr_char && isspace((unsigned char)*curr_char)){ //skip whitespaces
                curr_char++;
            }
            long val = strtol(curr_char, &end, 10); //read one int
            curr_char = end;
            m->values[r * cols + c] = (int)val; //store int into cell
        }
        while (*curr_char && isspace((unsigned char)*curr_char)){
            curr_char++;
        }
        if (*curr_char == ';'){ //end of row
            curr_char++;   
        }              
    }

    return m;
}

char* infix2postfix_sf(char *infix) {

    size_t len = strlen(infix);

    //allocate space for output string
    char *out = (char *)malloc(len + 1);
    if (out == NULL) return NULL;
    size_t out_len = 0;

    // keep operators on a stack while parsing
    char *opstk = (char *)malloc(len);
    if (opstk == NULL) { //allocation fails, free output buffer and return NULL
        free(out);
        return NULL;
    }
    ssize_t top = -1; //stack is empty

    for (size_t i = 0; i < len; i++) {
        char ch = infix[i];

        //skips white space
        if (isspace((unsigned char)ch)) continue;

        //if it's a matrix name, write to output
        if (ch >= 'A' && ch <= 'Z') {
            out[out_len++] = ch;
        }
        // ' transpose has highest precedence
        else if (ch == '\'') {
            out[out_len++] = '\'';
        }
        //( starts grouped expressions, so push onto stack
        else if (ch == '(') {
            opstk[++top] = '(';
        } 
        //pop operators until we hit matching ) then we remove (
        else if (ch == ')') {
            while (top >= 0 && opstk[top] != '(') {
                out[out_len++] = opstk[top--];
            }
            if (top >= 0 && opstk[top] == '(') top--;  
        } 
        //binary operator precedence
        else if (ch == '+' || ch == '*') {
            int prec;
            if(ch == '+'){
                prec = 1; 
            }
            else{
                prec = 2;
            }
            while (top >= 0 && (opstk[top] == '+' || opstk[top] == '*')) {
                int top_prec;
                if (opstk[top] == '+'){
                    top_prec = 1;
                }
                else{
                    top_prec = 2;
                }
                if (top_prec >= prec) {
                    out[out_len++] = opstk[top--];
                } 
                else {
                    break;
                }
            }
            opstk[++top] = ch; //push current operator 
        } 
    }

    //pop any remaining operators
    while (top >= 0) {
        if (opstk[top] != '(') out[out_len++] = opstk[top];
        top--;
    }

    out[out_len] = '\0';
    free(opstk);
    return out;
}


//helper for evaluate_expr_sf
static int is_temp_matrix(const matrix_sf *matrix_ptr){
    return !((matrix_ptr->name >= 'A') && (matrix_ptr->name <= 'Z'));
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) {

    char *post = infix2postfix_sf(expr); //convert infix string to postfix                     
    if (post == NULL) return NULL;

    //allocate stack of matrix pointers
    size_t max_line_size = strlen(post);
    size_t stack_size;
    if (max_line_size > 0){
        stack_size = max_line_size;
    }
    else{
        stack_size = 1; 
    }
    matrix_sf **stack = malloc(sizeof(matrix_sf *) * stack_size);
    if (stack == NULL) { 
        free(post); 
        return NULL; 
    }
    size_t top = 0; //current size of stack

    //going thorugh postfix char and ignore spaces
    for (size_t i = 0; post[i] != '\0'; ++i) {
        char ch = post[i];
        if (isspace((unsigned char)ch)) continue;

        if (ch >= 'A' && ch <= 'Z') { //operand find that matrix and push it 
            matrix_sf *m = find_bst_sf(ch, root);
            stack[top++] = m;
        }
        else if (ch == '\'') { //transpose pop one matrix
            matrix_sf *a = stack[--top];
            matrix_sf *m = transpose_mat_sf(a);
            if (!m) { 
                if (is_temp_matrix(a)){
                    free(a);
                }
                free(stack); 
                free(post);
                return NULL;
            }
            m->name = '?';  
            if (is_temp_matrix(a)){
                free(a);
            }
            stack[top++] = m;
        }
        else if (ch == '+' || ch == '*') { //binary operators pop right then left
            matrix_sf *b = stack[--top];
            matrix_sf *a = stack[--top];

            matrix_sf *r;
            if (ch == '+'){
                r = add_mats_sf(a,b);
            }
            else{
                r = mult_mats_sf(a,b);
            }
            if (!r) { 
                if (is_temp_matrix(a)){
                    free(a);
                }
                if (is_temp_matrix(b)){
                    free(b);
                }
                free(stack); 
                free(post);
                return NULL;
            }
            r->name = '?';  

            //free any popped matrices that were temps created during eval 
            if (is_temp_matrix(a)){
                free(a);
            }
            if (is_temp_matrix(b)){
                free(b);
            }

            stack[top++] = r;
        } 
    }

    matrix_sf *res;
    if (top > 0){
        res = stack[--top];
    }
    else{
        res = NULL;
    }

    if (res) res->name = name;//give result its name


    free(stack);                      
    free(post);                     
    return res;                    
}


//helper for execute_script_sf
static void free_bst_except(bst_sf *node, const matrix_sf *keep) {
    if (!node) return;
    free_bst_except(node->left_child,  keep);
    free_bst_except(node->right_child, keep);
    if (node->mat && node->mat != keep) {
        free(node->mat);  
    }
    free(node);
}


matrix_sf* execute_script_sf(char *filename) {

    char *str = NULL;
    FILE *file = fopen(filename, "r");
    if (file == NULL) return NULL;
    size_t max_line_size  = MAX_LINE_LEN;                 
    ssize_t got;

    bst_sf *root = NULL;           
    matrix_sf *last = NULL;           

    //read file with getline 
    while ((got = getline(&str, &max_line_size, file)) != -1) {          
        const char *curr_char = str; //pointer to go character by character 

        while (*curr_char && isspace((unsigned char)*curr_char)) curr_char++;
        if (!*curr_char) continue; //ignore spaces

        char name = *curr_char++; //lhs is single uppercase latter

        //find = 
        while (*curr_char && isspace((unsigned char)*curr_char)) curr_char++;
        if (*curr_char != '=') { 
            while (*curr_char && *curr_char != '=') {
                curr_char++; 
            }
        }
        else if (*curr_char == '='){
            curr_char++;
        }
        while (*curr_char && isspace((unsigned char)*curr_char)) {
            curr_char++; //start of rhs
        }

        //decide if rhs is a literal or a formula 
        int is_literal = (strchr(curr_char, '[') != NULL);

        matrix_sf *m;
        if (is_literal){ //if literal, parse 
            m = create_matrix_sf(name, curr_char);
        }
        else{ //if not, compute 
            m = evaluate_expr_sf(name, (char *)curr_char, root);
        }

        //get newly allocated matrix and insert it into bst 
        root = insert_bst_sf(m, root); 
        last = m; //keep track of last matrix to return 
    }

    //clean up file/str buffer
    free(str);
    fclose(file);

    //free the whole bst and all matrices except the one we returning
    free_bst_except(root, last);

    //return the final, named matrix from the last str 
    return last;
}



// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don'm touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
