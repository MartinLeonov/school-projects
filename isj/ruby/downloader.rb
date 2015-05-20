#!/usr/bin/env ruby

require 'optparse'
require 'net/http'
require 'open-uri'
require 'json'
require 'awesome_print'
require 'oauth'

# Load Twitter data, check for updates or download Tweet's link content
class Twitter
  def initialize(account)
    @oauth = {
      token: '258264341-nsqggXPTNwFIOlscZP9esF1UFLI1eV2tlROAtvc5',
      token_secret: '2qtBBD4fzxQEe1J57ZCoknKo42e7dfUPcLQqUeYYiKFdy',
      consumer_key: 'ml5JERXqTKv2GOmLIcYRXVtig',
      consumer_secret: '2CajnGudmSYVIXDRq0sdnooGwCUFP65Wo2uOHPM5LSxTwdLmlH'
    }
    @account = account
    @count = 50
    @prefix = "twitter-#{@account}"
    @file = JsonFile.new("#{@prefix}.json")
  end

  # Exchange oauth_token and oauth_token_secret
  # for an AccessToken instance.
  def prepare_access_token
    consumer = OAuth::Consumer.new(@oauth[:consumer_key],
                                   @oauth[:consumer_secret],
                                   site: 'https://api.twitter.com',
                                   scheme: :header)
    # create the access token object from passed values
    token_hash = { oauth_token: @oauth[:token],
                   oauth_token_secret: @oauth[:token_secret] }
    OAuth::AccessToken.from_hash(consumer, token_hash)
  end

  # Load necesary data from users public Twitter timeline
  def download_timeline
    access_token = prepare_access_token
    request = "screen_name=#{@account}&count=#{@count}"
    url = "https://api.twitter.com/1.1/statuses/user_timeline.json?#{request}"
    begin
      response = access_token.request(:get, url).body
      JSON.parse(response)
    rescue => e
      report_error "Connection error: #{e}.", 4
    end
  end

  # Filter only required data from loaded timeline
  def filter_data(timeline)
    timeline.map do |tweet|
      { id: tweet['id'],
        datetime: tweet['created_at'],
        text: tweet['text'],
        urls: unless tweet['entities']['urls'].empty?
                tweet['entities']['urls'].map { |url| url['expanded_url'] }
              end
      }
    end
  end

  # Recognize new tweets
  def new_tweets
    tweets = filter_data(download_timeline)
    new = if (stored = @file.read).empty?
            tweets
          else
            tweets.select { |tweet| tweet[:id] > stored.first['id'] }
          end
    puts "New tweets: #{new.count}"
    # append new tweets to file
    @file.write(new + stored) unless new.empty?
    new
  end

  # Load URLs for tweet with defined ID from logfile
  def urls_from_tweet(id)
    # load tweet data from logfile
    tweet = @file.read.select { |key| key['id'] == id.to_i }.first
    report_error 'No such ID in log file.', 3 if tweet.nil?
    # load URLs from tweet
    urls = tweet['urls'].each { |url| url }
    if urls.empty?
      report_error "No URL for this ID found in log file '#{@prefix}.json'.", 3
    end
    download_content(urls, id)
  end

  # Load URLs for every new tweet
  def urls_from_every_new_tweet
    return if (new = new_tweets).empty?
    new.each { |tweet| download_content(tweet[:urls], tweet[:id]) }
  end

  # Download content for URLs
  def download_content(urls, id)
    return if urls.nil?
    begin
      urls.each_with_index do |url, index|
        html = Net::HTTP.get(URI.parse(url))
        File.open("#{@prefix}-#{id}-#{index}.html", 'w') { |f| f.write(html) }
      end
      puts "Links for ID #{id} downloaded"
    rescue => e
      report_error "Connection error: #{e}.", 4
    end
  end

  # Print formated content of logfile
  def show
    if @file.read.empty?
      report_error "No log file for user '#{@account}'. Try 'load' first.", 2
    end
    ap @file.read, indent: 1
  end
end

# Load data from forum, can check for updates
class Forum
  DEFAULT_PAGE = 26
  NEXT_PAGE = 20
  POSTS_ON_PAGE = 20

  def initialize(url)
    @url = url
    @id = /[0-9]+$/.match(@url)
    @prefix = "forum-#{@id}"
    @file = JsonFile.new("#{@prefix}.json")
  end

  # Load all data from api
  def download_discussion
    content = Net::HTTP.get(URI.parse("#{@url}.json"))
    discussion = JSON.parse(content)
    highest_post_number = discussion['highest_post_number']
    puts 'Downloaded first posts'
    if highest_post_number > current_last(discussion)
      # more than 20 posts in thread
      pager = Forum::DEFAULT_PAGE
      loop do
        content = Net::HTTP.get(URI.parse("#{@url}/#{pager}.json"))
        # check how many posts are missing
        unloaded_count = highest_post_number - current_last(discussion)
        discussion['post_stream']['posts'].push(
          *JSON.parse(content)['post_stream']['posts'].last(unloaded_count)
        )
        downloaded_count = if unloaded_count > Forum::POSTS_ON_PAGE
                             Forum::POSTS_ON_PAGE
                           else
                             unloaded_count
                           end
        puts "Downloaded another #{downloaded_count} posts"
        pager += Forum::NEXT_PAGE
        break if highest_post_number == current_last(discussion)
      end
    end
    discussion
  end

  # Return number of last loaded post
  def current_last(posts)
    posts['post_stream']['posts'].last['post_number']
  end

  # Filter only required data from discussion
  def filter_data(discussion)
    { count: discussion['highest_post_number'],
      posts: discussion['post_stream']['posts'].map do |post|
               { post_number: post['post_number'],
                 username: post['username'],
                 datetime: post['created_at'],
                 text: post['cooked']
               }
             end
    }
  end

  # Recognize new posts in discussion
  def new_posts
    discussion = filter_data(download_discussion)
    stored = @file.read
    # recognize if new discussion were added
    case
    when stored.empty?
      new, new_count = discussion, discussion[:count]
    when stored['count'] < discussion[:count]
      new, new_count = discussion, (discussion[:count] - stored['count']).to_i
    else
      new, new_count = [], 0
    end
    puts "New posts in discussion: #{new_count}"
    # write discussion to file
    @file.write(new) unless new.empty?
  end

  def show
    if @file.read.empty?
      report_error 'No log file for this thread. Try loading data first.', 2
    end
    ap @file.read, indent: 1
  end
end

# Read and write json formated data to file
class JsonFile
  def initialize(filename)
    @filename = filename
  end

  # Load stored json data from file
  def read
    begin
      json_content = File.open(@filename, 'r') { |f| JSON.load(f) }
    rescue Errno::ENOENT
      json_content = []
    end
    json_content
  end

  # Write new json data to file
  def write(content)
    File.open(@filename, 'w') { |f| f.write(JSON.generate(content)) }
  end
end

# Parse command line arguments and choose propriet method
class Application
  def initialize
    @options = {}
    argparser
    check_args
    case @options[:interface]
    when :twitter
      twitter
    when :forum
      forum
    end
  end

  # Operate with twitter
  def twitter
    source = Twitter.new(@options[:account] || 'manageiq')
    case @options[:action]
    when :show
      source.show
    when :load
      source.new_tweets
    when :download
      if @options[:source].nil?
        source.urls_from_every_new_tweet
      else
        source.urls_from_tweet(@options[:source])
      end
    end
  end

  # Operate with forum
  def forum
    source = Forum.new(@options[:source])
    case @options[:action]
    when :show
      source.show
    when :load
      source.new_posts
    end
  end

  # Set parser for command line arguments
  def argparser
    ARGV << '-h' if ARGV.empty? # show help if no argument is passed
    options = OptionParser.new do |opts|
      opts.banner = 'Usage: downloader.rb [options]'
      opts.separator ''
      opts.separator 'Specific options:'
      opts.on('-i', '--interface INTERFACE', String, [:twitter, :forum],
              'Select interface to operate with (twitter, forum)') do |i|
        @options[:interface] = i
      end
      opts.on('-a', '--action ACTION', String, [:show, :load, :download],
              'Select action to do (show, load, download)') do |a|
        @options[:action] = a
      end
      opts.on('-s', '--source [SOURCE]', String,
              'Select source of data') do |s|
        @options[:source] = s
      end
      opts.on('-c', '--account [ACCOUNT]', String,
              'In case of twitter interface can be specified account') do |c|
        @options[:account] = c
      end
      opts.separator ''
      opts.separator 'Common options:'
      opts.on_tail('-h', '--help', 'Show this message') do
        puts opts
        exit
      end
    end
    parse_args(options)
  end

  # Parse arguments
  def parse_args(options)
    options.parse!
  rescue => e
    report_error "Wrong arguments: #{e}. Use --help or -h to show help.", 1
  end

  # Check if all arguments are set properly
  def check_args
    # check if interface and action are set
    mandatory = [:interface, :action]
    missing = mandatory.select { |param| @options[param].nil? }
    unless missing.empty?
      report_error "Missing options: #{missing.join(', ')}", 1
    end
    # forum interface is set, but not source or wrong action
    if @options[:interface] == :forum
      e = 'Source URL not specified.' if @options[:source].nil?
      e = 'No such action for forum.' if @options[:action] == :download
      unless e.nil?
        report_error "Wrong arguments: #{e} Use --help or -h to show help.", 1
      end
    end
  end
end

def report_error(message, exit_code)
  STDERR.puts message
  exit exit_code
end

Application.new
