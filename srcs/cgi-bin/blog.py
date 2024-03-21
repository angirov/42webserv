import os
import re
import sys
from urllib.parse import urlparse, parse_qs

BLOG_DIR = "/data/blog/"


sys.stderr.write("Python: starting the script.\n")

content = sys.stdin.read()

sys.stderr.write("Python: stdin: " + content + "\n")
for param in os.environ.keys():
    sys.stderr.write(f"{param}: {os.environ[param]} <br>")
sys.stderr.write("Content of stdin:")
sys.stderr.write(content)


def parse_query_string(query_string):
    parsed_query = urlparse("?" + query_string)
    query_params = parse_qs(parsed_query.query)

    title = query_params.get("title", [None])[0]
    date = query_params.get("date", [None])[0]

    if title is None or date is None:
        return None, None

    return title, date


def generate_blog_page(title, date, text):
    html_template = """
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>{title}</title>
    </head>
    <body>
        <header>
            <h1>{title}</h1>
            <p>Date: {date}</p>
        </header>
        <article>
            <p>{text}</p>
        </article>
        <footer>
            <p>© 2024 Best Webserv Team. All rights reserved.</p>
        </footer>
    </body>
    </html>
    """

    return html_template.format(title=title, date=date, text=text)


def generate_error_page(error_message):
    html_template = """
        <header>
            <h1>Error</h1>
        </header>
        <section>
            <p>{error_message}</p>
            <p>Please check the format of your request and try again.</p>
        </section>
    """
    return html_template.format(error_message=error_message)


def write_html_to_file(html_content, filename, dir_name):
    # Create the directory if it doesn't exist
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)

    # Construct the file path
    file_path = os.path.join(dir_name, filename)

    # Write the HTML content to the file
    with open(file_path, "w") as f:
        f.write(html_content)

    sys.stderr.write(f"HTML document written to {file_path}")


def check_input_format(input_string):
    if "text=" in input_string:
        return True
    else:
        return False


###################################################################################################

method = os.environ.get("REQUEST_METHOD")
if method != "POST":
    print(generate_error_page("Only POST method allowed for this script!", "<br>"))
###################################################################################################
else:
    if not content:
        print(generate_error_page("body missing"))
    qs = os.environ.get("QUERY_STRING")
    if check_input_format(content):
        text = parse_qs(content).get("text", [""])[0]
    else:
        text = content
    title, date = parse_query_string(qs)
    if not content or not title or not date:
        print(
            generate_error_page(
                "Request lacks body, or query string lacks title or date"
            )
        )
    else:
        file_name = date + "_" + title + ".html"
        blog = generate_blog_page(title, date, text)
        write_html_to_file(blog, file_name, BLOG_DIR)
        print("Content-Type: text/html\r\n\r\n", end="")
        print("<html> ")
        print("<head>")
        print("<title> Blog Generator </title>")
        print("</head>")
        print("<body>")
        print(
            'Generated article with the URL: <a href="{}">{}</a>'.format(
                "/blog/" + file_name, title
            )
        )
        print("</body>")
        print("</html>")
###################################################################################################


