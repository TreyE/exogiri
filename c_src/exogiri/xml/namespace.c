#include "exogiri.h"

/*
 * Stolen wholesale from Nokogiri.
 */
void recon_ns_after_move(Document* doc, xmlNodePtr tree, int recon_ns) {
  xmlNodePtr child;
  xmlAttrPtr attr;
  xmlNsPtr possible_collision_ns;
  int ns_collision;

  if (recon_ns && tree->doc) {
    xmlReconciliateNs(tree->doc, tree);
  }

  if (!(tree->parent)) {
    return;
  }
  if (!(tree->ns)) {
    return;
  }
  if (tree->nsDef) {
    xmlNsPtr curr = tree->nsDef;
    xmlNsPtr prev = NULL;

    while (curr) {
      xmlNsPtr ns = xmlSearchNsByHref(
                      tree->doc,
                      tree->parent,
                      curr->href
                    );
      /* Track and check for a namespace which might be 'squatting' on a
       * the same prefix but a different href. */
      ns_collision = 0;
      possible_collision_ns = xmlSearchNs(tree->doc, tree->parent, curr->prefix);
      if (possible_collision_ns && !xmlStrEqual(curr->href, possible_collision_ns->href)) {
        ns_collision = 1;
      }
      /* If we find the namespace is already declared, remove it from this
       * definition list. */
      if (ns && !ns_collision && ns != curr && xmlStrEqual(ns->prefix, curr->prefix)) {
        if (prev) {
          prev->next = curr->next;
        } else {
          tree->nsDef = curr->next;
        }
        // TODO: Pin the namespace for later deletion
        doc->unlinked_nses = pin_unlinked_ns(doc->unlinked_nses, curr);
        // pin_namespace()
      } else {
        prev = curr;
      }
      curr = curr->next;
    }
  }
  xmlNsPtr ns = xmlSearchNs(tree->doc, tree, tree->ns->prefix);
  if (ns
      && ns != tree->ns
      && xmlStrEqual(ns->prefix, tree->ns->prefix)
      && xmlStrEqual(ns->href, tree->ns->href)
      ) {
    xmlSetNs(tree, ns);
  }

  child = tree->children;
  while (NULL != child) {
    recon_ns_after_move(doc, child, 0);
    child = child->next;
  }

  if (tree->type == XML_ELEMENT_NODE) {
    attr = tree->properties;
    while (NULL != attr) {
      recon_ns_after_move(doc, (xmlNodePtr)attr, 0);
      attr = attr->next;
    }
  }
}