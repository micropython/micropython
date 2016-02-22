#ifndef _XML_H_
#define _XML_H_

struct xml_node {
	char *name;
	char *text;
	char *prefix;
	char *uri;
	char *attr;
	struct xml_node *parent;
	struct xml_node *child;
	struct xml_node *prev;
	struct xml_node *next;
};

struct xml_node_set {
	int count;
	struct xml_node **node;
};

void xml_free(void *buf);
int xml_doc_name(char *doc_buf, int doc_len, char **doc_prefix, char **doc_name, char **doc_uri);
struct xml_node *xml_parse_doc(char *doc_buf, int doc_len, char *prefix, char *doc_name, char *uri);
struct xml_node *xml_parse(char *doc_buf, int doc_len);
struct xml_node *xml_new_element(char *prefix, char *name, char *uri);
struct xml_node *xml_new_text(char *text);
int xml_is_element(struct xml_node *node);
int xml_is_text(struct xml_node *node);
struct xml_node* xml_copy_tree(struct xml_node *root);
void xml_delete_tree(struct xml_node *root);
void xml_add_child(struct xml_node *node, struct xml_node *child);
void xml_clear_child(struct xml_node *node);
struct xml_node* xml_text_child(struct xml_node *node);
void xml_set_text(struct xml_node *node, char *text);
struct xml_node_set* xml_find_element(struct xml_node *root, char *name);
struct xml_node_set* xml_find_path(struct xml_node *root, char *path);
void xml_delete_set(struct xml_node_set *node_set);
char *xml_dump_tree(struct xml_node *root);
char *xml_dump_tree_ex(struct xml_node *root, char *prolog, int new_line, int space);
void xml_set_attribute(struct xml_node *node, char *attr, char *value);
char *xml_get_attribute(struct xml_node *node, char *attr);

#endif
