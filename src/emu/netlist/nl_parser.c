// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nl_parser.c
 *
 */

#include "nl_parser.h"

//#undef NL_VERBOSE_OUT
//#define NL_VERBOSE_OUT(x) printf x

// ----------------------------------------------------------------------------------------
// A simple tokenizer
// ----------------------------------------------------------------------------------------

pstring ptokenizer::currentline_str()
{
    char buf[300];
    int bufp = 0;
    const char *p = m_line_ptr;
    while (*p && *p != 10)
        buf[bufp++] = *p++;
    buf[bufp] = 0;
    return pstring(buf);
}


void ptokenizer::skipeol()
{
    char c = getc();
    while (c)
    {
        if (c == 10)
        {
            c = getc();
            if (c != 13)
                ungetc();
            return;
        }
        c = getc();
    }
}


unsigned char ptokenizer::getc()
{
    if (*m_px == 10)
    {
        m_line++;
        m_line_ptr = m_px + 1;
    }
    if (*m_px)
        return *(m_px++);
    else
        return *m_px;
}

void ptokenizer::ungetc()
{
    m_px--;
}

void ptokenizer::require_token(const token_id_t &token_num)
{
    require_token(get_token(), token_num);
}

void ptokenizer::require_token(const token_t tok, const token_id_t &token_num)
{
    if (!tok.is(token_num))
    {
        error("Error: expected token <%s> got <%s>\n", m_tokens[token_num.id()].cstr(), tok.str().cstr());
    }
}

pstring ptokenizer::get_string()
{
    token_t tok = get_token();
    if (!tok.is_type(STRING))
    {
        error("Error: expected a string, got <%s>\n", tok.str().cstr());
    }
    return tok.str();
}

pstring ptokenizer::get_identifier()
{
    token_t tok = get_token();
    if (!tok.is_type(IDENTIFIER))
    {
        error("Error: expected an identifier, got <%s>\n", tok.str().cstr());
    }
    return tok.str();
}

ptokenizer::token_t ptokenizer::get_token()
{
    while (true)
    {
        token_t ret = get_token_internal();
        if (ret.is_type(ENDOFFILE))
            return ret;

        if (ret.is(m_tok_comment_start))
        {
            do {
                ret = get_token_internal();
            } while (ret.is_not(m_tok_comment_end));
        }
        else if (ret.is(m_tok_line_comment))
        {
            skipeol();
        }
        else if (ret.str() == "#")
        {
            skipeol();
        }
        else
            return ret;
    }
}

ptokenizer::token_t ptokenizer::get_token_internal()
{
    /* skip ws */
    char c = getc();
    while (m_whitespace.find(c)>=0)
    {
        c = getc();
        if (eof())
        {
            return token_t(ENDOFFILE);
        }
    }
    if (m_identifier_chars.find(c)>=0)
    {
        /* read identifier till non identifier char */
        pstring tokstr = "";
        while (m_identifier_chars.find(c)>=0) {
            tokstr += c;
            c = getc();
        }
        ungetc();
        token_id_t id = token_id_t(m_tokens.indexof(tokstr));
        if (id.id() >= 0)
            return token_t(id, tokstr);
        else
        {
            return token_t(IDENTIFIER, tokstr);
        }
    }
    else if (c == m_string)
    {
        pstring tokstr = "";
        c = getc();
        while (c != m_string)
        {
            tokstr += c;
            c = getc();
        }
        return token_t(STRING, tokstr);
    }
    else
    {
        /* read identifier till first identifier char or ws */
        pstring tokstr = "";
        while ((m_identifier_chars.find(c)) < 0 && (m_whitespace.find(c) < 0)) {
            tokstr += c;
            /* expensive, check for single char tokens */
            if (tokstr.len() == 1)
            {
                token_id_t id = token_id_t(m_tokens.indexof(tokstr));
                if (id.id() >= 0)
                    return token_t(id, tokstr);
            }
            c = getc();
        }
        ungetc();
        token_id_t id = token_id_t(m_tokens.indexof(tokstr));
        if (id.id() >= 0)
            return token_t(id, tokstr);
        else
        {
            return token_t(UNKNOWN, tokstr);
        }
    }

}

ATTR_COLD void ptokenizer::error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    pstring errmsg1 =pstring(format).vprintf(ap);
    va_end(ap);

    verror(errmsg1, currentline_no(), currentline_str());

    //throw error;
}

// ----------------------------------------------------------------------------------------
// A netlist parser
// ----------------------------------------------------------------------------------------

ATTR_COLD void netlist_parser::verror(pstring msg, int line_num, pstring line)
{
    m_setup.netlist().error("line %d: error: %s\n\t\t%s\n", line_num,
            msg.cstr(), line.cstr());

    //throw error;
}


void netlist_parser::parse(const char *buf)
{
    reset(buf);
    set_identifier_chars("abcdefghijklmnopqrstuvwvxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_.-");
    set_number_chars("01234567890eE-."); //FIXME: processing of numbers
    char ws[5];
    ws[0] = ' ';
    ws[1] = 9;
    ws[2] = 10;
    ws[3] = 13;
    ws[4] = 0;
    set_whitespace(ws);
    set_comment("/*", "*/", "//");
    m_tok_param_left = register_token("(");
    m_tok_param_right = register_token(")");
    m_tok_comma = register_token(",");

    m_tok_ALIAS = register_token("ALIAS");
    m_tok_NET_C = register_token("NET_C");
    m_tok_PARAM = register_token("PARAM");
    m_tok_NET_MODEL = register_token("PARAM");
    m_tok_NETLIST_START = register_token("NETLIST_START");
    m_tok_NETLIST_END = register_token("NETLIST_END");


	while (true)
	{
        token_t token = get_token();

        if (token.is_type(ENDOFFILE))
            return;

        require_token(m_tok_param_left);
        NL_VERBOSE_OUT(("Parser: Device: %s\n", n.cstr()));

        if (token.is(m_tok_ALIAS))
            net_alias();
        else if (token.is(m_tok_NET_C))
            net_c();
        else if (token.is(m_tok_PARAM))
            netdev_param();
        else if (token.is(m_tok_NET_MODEL))
            net_model();
        else if (token.is(m_tok_NETLIST_START))
            netdev_netlist_start();
        else if (token.is(m_tok_NETLIST_END))
            netdev_netlist_end();
        else
            device(token.str());
	}
}

void netlist_parser::netdev_netlist_start()
{
    // don't do much
    token_t name = get_token();
    require_token(m_tok_param_right);
}

void netlist_parser::netdev_netlist_end()
{
    // don't do much
    require_token(m_tok_param_right);
}

void netlist_parser::net_model()
{
    // don't do much
    pstring model = get_string();
    m_setup.register_model(model);
    require_token(m_tok_param_right);
}

void netlist_parser::net_alias()
{
	pstring alias = get_identifier();

	require_token(m_tok_comma);

	pstring out = get_identifier();

	require_token(m_tok_param_right);

	NL_VERBOSE_OUT(("Parser: Alias: %s %s\n", alias.cstr(), out.cstr()));
	m_setup.register_alias(alias, out);
}

void netlist_parser::net_c()
{
	pstring t1 = get_identifier();

	require_token(m_tok_comma);
	pstring t2 = get_identifier();

    require_token(m_tok_param_right);

	NL_VERBOSE_OUT(("Parser: Connect: %s %s\n", t1.cstr(), t2.cstr()));
	m_setup.register_link(t1 , t2);
}

void netlist_parser::netdev_param()
{
	pstring param;
	double val;
	param = get_identifier();
	require_token(m_tok_comma);
	val = eval_param(get_token());
	NL_VERBOSE_OUT(("Parser: Param: %s %f\n", param.cstr(), val));
	m_setup.register_param(param, val);
    require_token(m_tok_param_right);
}

void netlist_parser::device(const pstring &dev_type)
{
	pstring devname;
	net_device_t_base_factory *f = m_setup.factory().factory_by_name(dev_type, m_setup);
	netlist_device_t *dev;
	nl_util::pstring_list termlist = f->term_param_list();
	pstring def_param = f->def_param();
	token_t tok;

	int cnt;

	devname = get_identifier();

	dev = f->Create();
	m_setup.register_dev(dev, devname);

	NL_VERBOSE_OUT(("Parser: IC: %s\n", devname.cstr()));

	if (def_param != "")
	{
        pstring paramfq = devname + "." + def_param;
	    NL_VERBOSE_OUT(("Defparam: %s\n", def_param.cstr()));
	    require_token(m_tok_comma);

	    tok = get_token();
	    if (tok.is_type(STRING))
	    {
            m_setup.register_param(paramfq, tok.str());
	    }
	    else
	    {
	        double val = eval_param(tok);
	        m_setup.register_param(paramfq, val);
	    }
	}

	tok = get_token();
	cnt = 0;
	while (tok.is(m_tok_comma) && cnt < termlist.count())
	{
		pstring output_name = get_identifier();

		m_setup.register_link(devname + "." + termlist[cnt], output_name);

		cnt++;
	    tok = get_token();
	}
    if (cnt != termlist.count())
        fatalerror("netlist: input count mismatch for %s - expected %d found %d\n", devname.cstr(), termlist.count(), cnt);
    require_token(tok, m_tok_param_right);
}


// ----------------------------------------------------------------------------------------
// private
// ----------------------------------------------------------------------------------------


double netlist_parser::eval_param(const token_t tok)
{
	static const char *macs[6] = {"", "RES_K", "RES_M", "CAP_U", "CAP_N", "CAP_P"};
	static double facs[6] = {1, 1e3, 1e6, 1e-6, 1e-9, 1e-12};
	int i;
	int f=0;
	bool e;
	double ret;
	pstring val;

	//printf("param %s\n", tok.m_token.cstr());
	for (i=1; i<6;i++)
		if (tok.str().equals(macs[i]))
			f = i;
    if (f>0)
    {
        require_token(m_tok_param_left);
        val = get_identifier();
    }
    else
        val = tok.str();

    ret = val.as_double(&e);

    if (e)
		error("Error with parameter ...\n");
	if (f>0)
	    require_token(m_tok_param_right);
	return ret * facs[f];
}

