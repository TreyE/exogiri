#include "exogiri.h"

/*
 * Stolen wholesale from Nokogiri.
 */
void recon_ns_after_move(xmlDocPtr doc, xmlNodePtr tree, int depth) {
  xmlNodePtr child;
  xmlAttrPtr attr;

  if (depth == 0) {
    xmlReconciliateNs(doc, tree);
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
                      doc,
                      tree->parent,
                      curr->href
                    );
      int found_squatted_abbrev = 0;
      xmlNsPtr sameAbbrev = xmlSearchNs(doc, tree, curr->prefix);
      if (sameAbbrev) {
        if (!xmlStrEqual(sameAbbrev->href, curr->href)) {
          found_squatted_abbrev = 1;
        }
      }
      /* If we find the namespace is already declared, remove it from this
       * definition list. */
      if (ns && !found_squatted_abbrev && ns != curr && xmlStrEqual(ns->prefix, curr->prefix)) {
        if (prev) {
          prev->next = curr->next;
        } else {
          tree->nsDef = curr->next;
        }
      } else {
        prev = curr;
      }
      curr = curr->next;
    }
  }
  xmlNsPtr ns = xmlSearchNs(doc, tree, tree->ns->prefix);
  if (ns
      && ns != tree->ns
      && xmlStrEqual(ns->prefix, tree->ns->prefix)
      && xmlStrEqual(ns->href, tree->ns->href)
      ) {
    xmlSetNs(tree, ns);
  }

  child = tree->children;
  while (NULL != child) {
    recon_ns_after_move(doc, child, depth + 1);
    child = child->next;
  }

  if (tree->type == XML_ELEMENT_NODE) {
    attr = tree->properties;
    while (NULL != attr) {
      recon_ns_after_move(doc, (xmlNodePtr)attr, depth + 1);
      attr = attr->next;
    }
  }
}