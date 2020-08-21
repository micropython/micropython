{% if not obj.display %}
:orphan:

{% endif %}
:mod:`{{ obj.name }}`
======={{ "=" * obj.name|length }}

.. py:module:: {{ obj.name }}

{% if obj.docstring %}
.. autoapi-nested-parse::

   {{ obj.docstring|prepare_docstring|indent(3) }}

{% endif %}

{% block subpackages %}
{% set visible_subpackages = obj.subpackages|selectattr("display")|list %}
{% if visible_subpackages %}
.. toctree::
   :maxdepth: 2

{% for subpackage in visible_subpackages %}
   {{ subpackage.short_name }}/index.rst
{% endfor %}


{% endif %}
{% endblock %}
{% block submodules %}
{% set visible_submodules = obj.submodules|selectattr("display")|list %}
{% if visible_submodules %}

.. toctree::
   :titlesonly:
   :maxdepth: 1

{% for submodule in visible_submodules %}
   {{ submodule.short_name }}/index.rst
{% endfor %}


{% endif %}
{% endblock %}
{% block content %}
{% if obj.all is not none %}
{% set visible_children = obj.children|selectattr("short_name", "in", obj.all)|list %}
{% elif obj.type is equalto("package") %}
{% set visible_children = obj.children|selectattr("display")|list %}
{% else %}
{% set visible_children = obj.children|selectattr("display")|rejectattr("imported")|list %}
{% endif %}
{% if visible_children %}

{% set visible_classes = visible_children|selectattr("type", "equalto", "class")|list %}
{% set visible_functions = visible_children|selectattr("type", "equalto", "function")|list %}
{% if "show-module-summary" in autoapi_options and (visible_classes or visible_functions) %}
{% block classes %}
{% if visible_classes %}
Classes
~~~~~~~

.. autoapisummary::

{% for klass in visible_classes %}
   {{ klass.id }}
{% endfor %}


{% endif %}
{% endblock %}

{% block functions %}
{% if visible_functions %}
Functions
~~~~~~~~~

.. autoapisummary::

{% for function in visible_functions %}
   {{ function.id }}
{% endfor %}


{% endif %}
{% endblock %}
{% endif %}
{% for obj_item in visible_children %}
{{ obj_item.rendered|indent(0) }}
{% endfor %}
{% endif %}
{% endblock %}
